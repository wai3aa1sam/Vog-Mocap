#pragma once

#include "Vog/Core/Core.h"

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"	// must include (see spdlog doc)

namespace vog {

	class VOG_API Log
	{
	public:
		inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return s_coreLogger; }
		inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return s_clientLogger; }

		static void init();

	private:
		Log(const Log&) = delete;
		void operator=(const Log&) = delete;

	private:
		static std::shared_ptr<spdlog::logger> s_coreLogger;
		static std::shared_ptr<spdlog::logger> s_clientLogger;
	};
}

#pragma region Log_marcos

#if (defined VOG_DEBUG) || (defined VOG_FORCE_LOG) 

// Core Log
#define VOG_CORE_LOG_TRACE(...)		::vog::Log::getCoreLogger()->trace(__VA_ARGS__)
#define VOG_CORE_LOG_INFO(...)		::vog::Log::getCoreLogger()->info(__VA_ARGS__)
#define VOG_CORE_LOG_WARN(...)		::vog::Log::getCoreLogger()->warn(__VA_ARGS__)
#define VOG_CORE_LOG_ERROR(...)		::vog::Log::getCoreLogger()->error(__VA_ARGS__)
#define VOG_CORE_LOG_CRITICAL(...)	::vog::Log::getCoreLogger()->critical(__VA_ARGS__)

// Application Log
#define VOG_LOG_TRACE(...)			::vog::Log::getClientLogger()->trace(__VA_ARGS__)
#define VOG_LOG_INFO(...)			::vog::Log::getClientLogger()->info(__VA_ARGS__)
#define VOG_LOG_WARN(...)			::vog::Log::getClientLogger()->warn(__VA_ARGS__)
#define VOG_LOG_ERROR(...)			::vog::Log::getClientLogger()->error(__VA_ARGS__)
#define VOG_LOG_CRITICAL(...)		::vog::Log::getClientLogger()->critical(__VA_ARGS__)

#else
	#define VOG_CORE_LOG_TRACE(...)
	#define VOG_CORE_LOG_INFO(...)	
	#define VOG_CORE_LOG_WARN(...)	
	#define VOG_CORE_LOG_ERROR(...)
	#define VOG_CORE_LOG_CRITICAL(...)

	#define VOG_LOG_TRACE(...)	
	#define VOG_LOG_INFO(...)	
	#define VOG_LOG_WARN(...)	
	#define VOG_LOG_ERROR(...)	
	#define VOG_LOG_CRITICAL(...)	

	#define VOG_CORE_LOG_GLM(glm)
#endif // VOG_DEBUG
#pragma endregion


