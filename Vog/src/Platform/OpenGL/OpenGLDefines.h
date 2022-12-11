#pragma once
#include "Vog/Core/Core.h"
#include "Vog/Core/Log.h"
#include <glad/glad.h>

#include <algorithm>

namespace vog {

	struct ErrorCache
	{
		constexpr static int s_max_error_print_size = 6;
		GLenum errorCode;
		int lastErrorLineNumber = 0;
		const char* pFileName = nullptr;
		ErrorCache(GLenum errorCode_, int lastErrorLineNumber_, const char* pFileName_)
			:
			pFileName(pFileName_)
		{
			errorCode = errorCode_;
			lastErrorLineNumber = lastErrorLineNumber_;
		}
	};

	static GLenum glCheckError_(const char* file, int line)
	{
		static std::vector<ErrorCache> errorCaches;

		GLenum errorCode;
		while ((errorCode = glGetError()) != GL_NO_ERROR)
		{
			static std::string error;

			switch (errorCode)
			{
				case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
				case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
				case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
				case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
				case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
				case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
				case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
			}
			
			auto it = std::find_if(errorCaches.begin(), errorCaches.end(), [file, line, errorCode](ErrorCache errorCache_)
				{
					return errorCache_.lastErrorLineNumber == line && errorCache_.errorCode == errorCode && strcmp(errorCache_.pFileName, file) == 0;
				});
			if (it == errorCaches.end() /*&& errorCaches.size() < ErrorCache::s_max_error_print_size*/)
			{
				errorCaches.emplace_back(errorCode, line, file);
				VOG_CORE_LOG_ERROR("ERROR: {0} | path: {1}, line: {2} \n====================", error, file, line);

				s_is_reset_error = false;
			}
			return errorCode;
		}

		if (s_is_reset_error)
		{
			errorCaches.clear();
		}

		return errorCode;
	}
#ifdef VOG_ENABLE_OPENGL_DEBUG
	#define glCheckError() glCheckError_(__FILE__, __LINE__) 
	#define glDebugCall(x) \
		x; glCheckError(); \
		//---------------------
#else
	#define glCheckError()
	#define glDebugCall(x) x;
#endif // VOG_DEBUG

#pragma region OpenGL_API_Stuff
	#define WGL_CONTEXT_COREPROFILE_BIT_ARB 0x00000001
	typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);

	typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC)(void);
	typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC)(int);
	typedef int (WINAPI* PFNWGLGETSWAPINTERVALEXTPROC) (void);

	typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
	typedef BOOL(WINAPI* PFNWGLMAKECONTEXTCUREENTARBPROC) (HDC hDrawDC, HDC hReadDC, HGLRC hglrc);

	//extern void (*wglMakeContextCurrentARB)(HDC hDrawDC, HDC hReadDC, HGLRC hglrc)

	#define WGL_NUMBER_PIXEL_FORMATS_ARB 0x2000
	#define WGL_DRAW_TO_WINDOW_ARB 0x2001
	#define WGL_DRAW_TO_BITMAP_ARB 0x2002
	#define WGL_ACCELERATION_ARB 0x2003
	#define WGL_NEED_PALETTE_ARB 0x2004
	#define WGL_NEED_SYSTEM_PALETTE_ARB 0x2005
	#define WGL_SWAP_LAYER_BUFFERS_ARB 0x2006
	#define WGL_SWAP_METHOD_ARB 0x2007
	#define WGL_NUMBER_OVERLAYS_ARB 0x2008
	#define WGL_NUMBER_UNDERLAYS_ARB 0x2009
	#define WGL_TRANSPARENT_ARB 0x200A
	#define WGL_SHARE_DEPTH_ARB 0x200C
	#define WGL_SHARE_STENCIL_ARB 0x200D
	#define WGL_SHARE_ACCUM_ARB 0x200E
	#define WGL_SUPPORT_GDI_ARB 0x200F
	#define WGL_SUPPORT_OPENGL_ARB 0x2010
	#define WGL_DOUBLE_BUFFER_ARB 0x2011
	#define WGL_STEREO_ARB 0x2012
	#define WGL_PIXEL_TYPE_ARB 0x2013
	#define WGL_COLOR_BITS_ARB 0x2014
	#define WGL_RED_BITS_ARB 0x2015
	#define WGL_RED_SHIFT_ARB 0x2016
	#define WGL_GREEN_BITS_ARB 0x2017
	#define WGL_GREEN_SHIFT_ARB 0x2018
	#define WGL_BLUE_BITS_ARB 0x2019
	#define WGL_BLUE_SHIFT_ARB 0x201A
	#define WGL_ALPHA_BITS_ARB 0x201B
	#define WGL_ALPHA_SHIFT_ARB 0x201C
	#define WGL_ACCUM_BITS_ARB 0x201D
	#define WGL_ACCUM_RED_BITS_ARB 0x201E
	#define WGL_ACCUM_GREEN_BITS_ARB 0x201F
	#define WGL_ACCUM_BLUE_BITS_ARB 0x2020
	#define WGL_ACCUM_ALPHA_BITS_ARB 0x2021
	#define WGL_DEPTH_BITS_ARB 0x2022
	#define WGL_STENCIL_BITS_ARB 0x2023
	#define WGL_AUX_BUFFERS_ARB 0x2024
	#define WGL_NO_ACCELERATION_ARB 0x2025
	#define WGL_GENERIC_ACCELERATION_ARB 0x2026
	#define WGL_FULL_ACCELERATION_ARB 0x2027
	#define WGL_SWAP_EXCHANGE_ARB 0x2028
	#define WGL_SWAP_COPY_ARB 0x2029
	#define WGL_SWAP_UNDEFINED_ARB 0x202A
	#define WGL_TYPE_RGBA_ARB 0x202B
	#define WGL_TYPE_COLORINDEX_ARB 0x202C
	#define WGL_TRANSPARENT_RED_VALUE_ARB 0x2037
	#define WGL_TRANSPARENT_GREEN_VALUE_ARB 0x2038
	#define WGL_TRANSPARENT_BLUE_VALUE_ARB 0x2039
	#define WGL_TRANSPARENT_ALPHA_VALUE_ARB 0x203A
	#define WGL_TRANSPARENT_INDEX_VALUE_ARB 0x203B

	#ifndef WGL_ARB_multisample
	#define WGL_ARB_multisample 1

	#define WGL_SAMPLE_BUFFERS_ARB 0x2041
	#define WGL_SAMPLES_ARB 0x2042

	#define WGLEW_ARB_multisample WGLEW_GET_VAR(__WGLEW_ARB_multisample)

	#endif /* WGL_ARB_multisample */

	#ifndef WGL_ARB_create_context_profile
	#define WGL_ARB_create_context_profile 1

	#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
	#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
	#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

	#define WGLEW_ARB_create_context_profile WGLEW_GET_VAR(__WGLEW_ARB_create_context_profile)

	#endif /* WGL_ARB_create_context_profile */

	#ifndef WGL_ARB_create_context
	#define WGL_ARB_create_context 1

	#define WGL_CONTEXT_DEBUG_BIT_ARB 0x0001
	#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002
	#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
	#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
	#define WGL_CONTEXT_LAYER_PLANE_ARB 0x2093
	#define WGL_CONTEXT_FLAGS_ARB 0x2094
	#define ERROR_INVALID_VERSION_ARB 0x2095
	#define ERROR_INVALID_PROFILE_ARB 0x2096
	#endif
#pragma endregion



}