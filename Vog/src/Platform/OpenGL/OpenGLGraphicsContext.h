#pragma once

#include "Vog/Core/Core.h"

#include "Vog/Graphics/GraphicsContext.h"
#include "Platform/Windows/WindowsWindowData.h"

//------- References
// https://github.com/Eversmile12/OpenGLNative/blob/master/OpenGL/OpenGL/src/WinMain.cpp
// https://github.com/SimpleTalkCpp/SimpleTalkCpp_Tutorial/blob/main/Graphic/Advance%20OpenGL/Advance%20OpenGL%20003/MyOpenGLWindow.h

namespace vog {

	class OpenGLGraphicsContext : public GraphicsContext
	{
	public:
		OpenGLGraphicsContext(WindowData* pWindowData_);
		virtual ~OpenGLGraphicsContext();

		virtual void init() override;
		virtual void swapBuffers() override;
		virtual void makeContextCurrent() override;

		virtual void enableVsync(bool isVsync_) override;
		inline virtual void* getHandle() const override { return m_hrc; }
	private:
		void _createContext();

	private:
		HGLRC	m_hrc = nullptr;
		WindowData* m_pWindowData = nullptr;
	};


#pragma region OpenGLWindow_FalseContext
	class OpenGLWindow_FalseContext : public NonCopyable {
	public:

		~OpenGLWindow_FalseContext() {
			if (m_rc) { ::wglDeleteContext(m_rc);		m_rc = nullptr; }
			if (m_dc) { ::ReleaseDC(m_hwnd, m_dc);	m_dc = nullptr; }
			if (m_hwnd) { ::DestroyWindow(m_hwnd);		m_hwnd = nullptr; }
		}

		void create() {
			auto hInstance = GetModuleHandle(nullptr);

			const wchar_t* className = L"MyOpenGLWindow_FalseContext";

			//-- Check did we register window class yet
			WNDCLASSEX wc;
			if (!GetClassInfoEx(hInstance, className, &wc))
			{
				//-- register window class
				memset(&wc, 0, sizeof(wc));
				wc.cbSize = sizeof(wc);
				wc.style = CS_OWNDC; //!! <------- CS_OWNDC is required for OpenGL Window
				wc.lpfnWndProc = DefWindowProc;
				wc.hInstance = hInstance;
				wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
				wc.hbrBackground = nullptr; //!! <------- 
				wc.lpszClassName = className;

				if (!RegisterClassEx(&wc))
				{
					VOG_CORE_LOG_ERROR("RegisterClassEx");
				}
			}

			m_hwnd = CreateWindowExW(0,
				className,
				L"",
				WS_POPUP,
				0, 0, 0, 0,
				nullptr, nullptr, hInstance, nullptr);
			if (!m_hwnd) {
				VOG_CORE_LOG_ERROR("CreateWindow");
			}

			m_dc = GetDC(m_hwnd);
			if (!m_dc) {
				VOG_CORE_LOG_ERROR("GetDC");
			}

			PIXELFORMATDESCRIPTOR pfd;
			memset(&pfd, 0, sizeof(pfd));
			pfd.nSize = sizeof(pfd);
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = 32;
			pfd.cDepthBits = 32;
			pfd.iLayerType = PFD_MAIN_PLANE;

			int nPixelFormat = ChoosePixelFormat(m_dc, &pfd);
			if (nPixelFormat == 0) {
				VOG_CORE_LOG_ERROR("ChoosePixelFormat");
			}

			BOOL bResult = SetPixelFormat(m_dc, nPixelFormat, &pfd);
			if (!bResult) {
				VOG_CORE_LOG_ERROR("SetPixelFormat");
			}

			m_rc = wglCreateContext(m_dc);
			if (!m_rc) {
				VOG_CORE_LOG_ERROR("wglCreateContext");
			}

			wglMakeCurrent(m_dc, m_rc);
		}

		HGLRC getRC() const { return m_rc; }


	private:
		HWND	m_hwnd = nullptr;
		HDC		m_dc = nullptr;
		HGLRC	m_rc = nullptr;
	};
#pragma endregion

}