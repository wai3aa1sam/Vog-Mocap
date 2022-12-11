#pragma once
#include <memory>
#include <filesystem>

// search guideline
// ==Vog_Static_Varaible==

#ifdef VOG_PLATFORM_WINDOWS
	#ifdef VOG_DYNAMIC_LINK
		#ifdef VOG_BUILD_DLL
			#define VOG_API __declspec(dllexport)
		#else
			#define VOG_API __declspec(dllimport)
		#endif // VOG_BUILD_DLL
	#else
		#define VOG_API
	#endif
#else
	#error VOG only support Windows!
#endif // VOG_PLATFORM_WIDNOWS

#ifdef VOG_DEBUG
	#define VOG_ENABLE_ASSERT
	#define VOG_ENABLE_PRINT_MATH
	#define VOG_ENABLE_OPENGL_DEBUG
#else
	#define VOG_ENABLE_PRINT_MATH

#endif // VOG_DEBUG

#ifdef VOG_ENABLE_ASSERT
	#define VOG_CORE_ASSERT(x, ...) if(!(x)) { VOG_CORE_LOG_ERROR("Assertion Failed: {2}, File: {0}, Line: {1}", __FILE__, __LINE__, __VA_ARGS__);	__debugbreak(); }
	#define VOG_ASSERT(x, ...) if(!(x)) { VOG_LOG_ERROR("Assertion Failed: {2}, File: {0}, Line: {1}", __FILE__, __LINE__, __VA_ARGS__);			__debugbreak(); }
#else
	#define VOG_CORE_ASSERT(x, ...)
	#define VOG_ASSERT(x, ...)
#endif // VOG_ENABLE_ASSERT

#define BIT(x) (1 << (x))
#define BIND_EVENT_CALLBACK(fn) (std::bind(&fn, this, std::placeholders::_1))
#define VOG_FORCE_LOG

#pragma region misc_define
//#define imgui_default_docking		// default dokcing branch in ImGui seems has bug on OpenGL3 with win32, imgui window will be black if outside
// 

// debug stuff
//#define debug_Shader						in Shader.h
//#define debug_ShaderLayout				in ShaderLayout.h
//#define debug_memory_buffer				in MemoryBuffer.h
//#define VOG_ENABLE_SHAPE_NAME				in Shape.h

// temp flag
static bool s_is_reset_error = false;				// TODO: remove
#pragma endregion


namespace vog {
	using filepath = std::filesystem::path;

	template<typename T>
	using RefPtr = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr RefPtr<T> createRefPtr(Args&& ... args_)
	{
		return std::make_shared<T>(std::forward<Args>(args_)...);
	}

	template<typename T>
	using ScopePtr = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr ScopePtr<T> createScopePtr(Args&& ... args_)
	{
		return std::make_unique<T>(std::forward<Args>(args_)...);
	}
}