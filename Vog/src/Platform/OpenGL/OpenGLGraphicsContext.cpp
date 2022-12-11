#include "vogpch.h"
#include "Platform/OpenGL/OpenGLGraphicsContext.h"
#include "Platform/OpenGL/OpenGLDefines.h"

namespace vog {
	OpenGLGraphicsContext::OpenGLGraphicsContext(WindowData* pWindowData_)
		:
		m_pWindowData(pWindowData_)
	{

	}

	OpenGLGraphicsContext::~OpenGLGraphicsContext()
	{
		if (m_hrc) 
		{ 
			wglDeleteContext(m_hrc);	
			m_hrc = nullptr; 
		}
	}

	void OpenGLGraphicsContext::init()
	{
		//---------  OpenGL Flase Context
		//--------- create old style OpenGL context and load the new verion
		OpenGLWindow_FalseContext falseContext;
		falseContext.create();

		int status = gladLoadGL();

		VOG_CORE_ASSERT(status, "Failed to initialize Glad!");

		_createContext();

		VOG_CORE_LOG_INFO("OpenGL Renderer:");
		VOG_CORE_LOG_INFO("	Vendor: {0}", glGetString(GL_VENDOR));
		VOG_CORE_LOG_INFO("	Renderer: {0}", glGetString(GL_RENDERER));
		VOG_CORE_LOG_INFO("	Version: {0}", glGetString(GL_VERSION));

		GLint isMultiSample = 0;
		int texture_units = 0;
		glGetIntegerv(GL_SAMPLE_BUFFERS, &isMultiSample);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
		VOG_CORE_LOG_INFO("max texture size: {0}", texture_units);

		isMultiSample ? VOG_CORE_LOG_INFO("able to multi sample") : VOG_CORE_LOG_INFO("unable to multi sample");
	}

	void OpenGLGraphicsContext::swapBuffers()
	{
		if (m_pWindowData->hdc)
			::SwapBuffers(m_pWindowData->hdc);
	}

	void OpenGLGraphicsContext::makeContextCurrent()
	{
		PFNWGLMAKECONTEXTCUREENTARBPROC wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCUREENTARBPROC)wglGetProcAddress("wglMakeContextCurrentARB");
		if (m_hrc)
		{
			wglMakeContextCurrentARB(m_pWindowData->hdc, m_pWindowData->hdc, m_hrc);
			return;
		}
		VOG_CORE_LOG_ERROR("no Context");
	}

	void OpenGLGraphicsContext::enableVsync(bool isVsync_)
	{
		PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGEXTPROC>(wglGetProcAddress("wglGetExtensionsStringEXT"));
		bool swapControlSupported = strstr(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != 0;
		// https://www.khronos.org/opengl/wiki/Swap_Interval

		if (swapControlSupported) {
			PFNWGLSWAPINTERVALEXTPROC wglSwapInternalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
			PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
			if (isVsync_)
			{
				wglSwapInternalEXT(1);
				VOG_CORE_LOG_INFO("VSynch enabled");
			}
			else
			{
				wglSwapInternalEXT(0);
				VOG_CORE_LOG_INFO("VSynch disabled");
			}
		}
		else
		{
			VOG_CORE_LOG_INFO("WGL_EXT_swap_control not supported");
		}
		m_pWindowData->isVsync = isVsync_;
	}

	void OpenGLGraphicsContext::_createContext()
	{
		const int formatAttrs[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			// Multisampling only works when double-buffering is enabled
			WGL_SAMPLE_BUFFERS_ARB,		GL_TRUE,
			WGL_SAMPLES_ARB,			8,
			0 // End of attributes list
		};

		int contextAttrs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 6,

	#if 0 // disable deprecated function, such as glBegin, glColor, glLoadMatrix, glPushMatrix
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
	#else
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
	#endif

			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0 // End of attributes list
		};

		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

		int format, numFormat;
		if (!wglChoosePixelFormatARB(m_pWindowData->hdc, formatAttrs, nullptr, 1, &format, (UINT*)&numFormat))
		{
			VOG_CORE_LOG_ERROR("wglChoosePixelFormatARB");
		}

		PIXELFORMATDESCRIPTOR pfd;
		if (!SetPixelFormat(m_pWindowData->hdc, format, &pfd)) 
		{
			VOG_CORE_LOG_ERROR("SetPixelFormat");
		}

		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
		HGLRC sharedContext = nullptr;
		m_hrc = wglCreateContextAttribsARB(m_pWindowData->hdc, sharedContext, contextAttrs);
		if (!m_hrc) {

			VOG_CORE_LOG_ERROR("wglCreateContext");
		}

		PFNWGLMAKECONTEXTCUREENTARBPROC wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCUREENTARBPROC)wglGetProcAddress("wglMakeContextCurrentARB");
		if (m_hrc)
		{
			wglMakeContextCurrentARB(m_pWindowData->hdc, m_pWindowData->hdc, m_hrc);
		}
	}
}