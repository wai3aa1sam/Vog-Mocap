#pragma once
#include "Vog/Core/Log.h"

#include <limits>
#include <stdint.h>

#ifdef VOG_PLATFORM_WINDOWS
#include <Windows.h>
#include <Windowsx.h>
#endif // VOG_PLATFORM_WINDOWS

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>


// getToken / Noncopyable...
// Reference: https://github.com/SimpleTalkCpp/SimpleTalkCpp_Tutorial/blob/main/Graphic/Advance%20OpenGL/Advance%20OpenGL%20003/my_common.h

namespace vog {

	class NonCopyable
	{
	public:
		NonCopyable() = default;
	private:
		NonCopyable(const NonCopyable&) = delete;
		void operator=(const NonCopyable&) = delete;
	};

	inline uint64_t my_to_uint64(const LARGE_INTEGER& v) {
		return static_cast<uint64_t>(v.HighPart) << 32 | v.LowPart;
	}

	inline void my_readFile(std::string& outStr_, const std::string& filepath_)
	{
		outStr_.clear();
		std::ifstream file(filepath_, std::ios::in | std::ios::binary );
		if (file.is_open())
		{
			file.seekg(0, std::ios::end);
			size_t size = file.tellg();
			if (size != -1)
			{
				outStr_.resize(size);
				file.seekg(0, std::ios::beg);
				file.read(&outStr_[0], size);
			}
			file.close();
		}
		else
		{
			VOG_CORE_LOG_ERROR("Cannot open shader file, path: {0}", filepath_);
		}
	}

	inline const char* my_getToken(std::string& outStr_, const char* start_, char delimiter_ = ' ')
	{
		outStr_.clear();
		if (!start_)
			return nullptr;

		// trim spaces
		for (; *start_ != '\0'; ++start_)
		{
			if (*start_ != ' ')
				break;
		}

		// abcdefghijkl 
		// ^ - start_  ^ - last
		auto* last = strchr(start_, delimiter_);	// get the first delimiter
		if (!last)
		{
			outStr_ = start_;
			return nullptr;
		}
		else
		{
			outStr_.assign(start_, last);
			return last + 1;
		}
	}

	inline uint64_t my_getToken(std::string& outStr_, const std::string& str_, uint64_t start_, std::string_view delimiter_ = " ")
	{
		outStr_.clear();
		if (str_.empty() || start_ > str_.size())
			return std::string::npos;

		// trim spaces
		start_ = str_.find_first_not_of(' ', start_);

		// abcdefghijkl 
		// ^ - start_  ^ - last
		uint64_t last = str_.find(delimiter_, start_);
		if (last != std::string::npos)
		{
			outStr_.reserve(last - start_);
			outStr_ = str_.substr(start_, last - start_);
			return last + 1;
		}
		else
		{
			return std::string::npos;
		}
	}

	static std::string getIndentationLevel(uint32_t level_)
	{
		std::string indentation = "";
		for (uint32_t i = 0; i < level_; i++)
			indentation += "--";
		return indentation;
	}
}