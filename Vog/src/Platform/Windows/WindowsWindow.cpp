#include "vogpch.h"
#include "WindowsWindow.h"

#include "Platform/OpenGL/OpenGLGraphicsContext.h"
#include "Platform/OpenGL/OpenGLDefines.h"

#include "Vog/Events/ApplicationEvent.h"
#include "Vog/Events/KeyEvent.h"
#include "Vog/Events/MouseEvent.h"

#include <backends/imgui_impl_win32.h>

// win32 msg handle
// reference to glfw, win32_window.c

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace vog {

	WindowsWindow::WindowsWindow(const WindowBasicData& windowBasicData_)
	{
		_init(windowBasicData_);

		//---------  OpenGL Context
		m_pContext = createScopePtr<OpenGLGraphicsContext>(&m_windowData);
		m_pContext->init();

		ShowWindow(m_windowData.hwnd, SW_SHOW);
		UpdateWindow(m_windowData.hwnd);
	}

	WindowsWindow::~WindowsWindow()
	{
		// destroy context first!
		_shutDown();
	}

	void WindowsWindow::_shutDown()
	{
		if (m_windowData.hdc)
		{
			ReleaseDC(m_windowData.hwnd, m_windowData.hdc);
			m_windowData.hdc = nullptr;
		}
		if (m_windowData.hwnd)
		{
			DestroyWindow(m_windowData.hwnd);
			m_windowData.hwnd = nullptr;
		}
	}

	void WindowsWindow::onUpdate()
	{
		WindowsWindow::_pollEvents();
		m_pContext->swapBuffers();
	}

	void WindowsWindow::setAppOnEventCallBackFn(AppOnEventCallBackFn fn_)
	{
		m_windowData.appOnEventCallBackFn = fn_;
	}

	inline uint32_t WindowsWindow::getWidth() const
	{
		return m_windowData.windowBasicData.width;
	}

	inline uint32_t WindowsWindow::getHeight() const
	{
		return m_windowData.windowBasicData.height;
	}

	void WindowsWindow::setVsync(bool isVsync_)
	{
		m_pContext->enableVsync(isVsync_);
		m_windowData.isVsync = isVsync_;
	}

	inline bool WindowsWindow::isVsync() const
	{
		return m_windowData.isVsync;
	}

	void WindowsWindow::_init(const WindowBasicData& windowBasicData_)
	{
		m_windowData.windowBasicData.name = windowBasicData_.name;
		m_windowData.windowBasicData.width = windowBasicData_.width;
		m_windowData.windowBasicData.height = windowBasicData_.height;

		auto hInstance = GetModuleHandle(nullptr);

		WNDCLASSEX wc;
		wchar_t* className = (wchar_t*)m_windowData.windowBasicData.name.c_str();
		if (!GetClassInfoEx(hInstance, className, &wc))
		{
			//-- register window class
			memset(&wc, 0, sizeof(wc));
			wc.cbSize = sizeof(wc);
			wc.style = CS_OWNDC; //!! <------- CS_OWNDC is required for OpenGL Window
			wc.lpfnWndProc = _wndProc;
			wc.hInstance = hInstance;
			wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wc.hbrBackground = nullptr; //!! <------- 
			wc.lpszClassName = className;

			if (!RegisterClassEx(&wc))
			{
				VOG_CORE_LOG_ERROR("RegisterClassEx");
			}
		}

		m_windowData.hwnd = CreateWindowEx(0,
			className,
			L"Vog Engine",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 1280, 768,
			nullptr, nullptr, hInstance,
			this); //!! <----- pass this to WM_CREATE

		if (!m_windowData.hwnd)
		{
			VOG_CORE_LOG_ERROR("CreateWindow");
		}

		m_windowData.hdc = GetDC(m_windowData.hwnd);
		if (!m_windowData.hdc)
		{
			VOG_CORE_LOG_ERROR("GetDC");
		}
	}

	LRESULT WINAPI WindowsWindow::_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		{
			//return true;
		}
		//ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);

		switch (msg)
		{
			case WM_CREATE: {
				auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
				auto* thisObj = reinterpret_cast<WindowsWindow*>(cs->lpCreateParams);
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)thisObj);
			}break;

			case WM_DESTROY: {
				//getThis(hwnd)->onDestroy();
			}break;

			case WM_CLOSE: {
				auto* thisObj = _getThis(hwnd);
				thisObj->_onCloseCallback();
				return 1;
			}break;

			case WM_SIZE: {
				auto* thisObj = _getThis(hwnd);
				auto w = GET_X_LPARAM(lParam);
				auto h = GET_Y_LPARAM(lParam);
				thisObj->_onResizeCallback(w, h);
			}break;

			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:	
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:	
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:	
			{
				KeyState state;
				KeyCode button;
				if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
					button = KeyCode::LButton;
				else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
					button = KeyCode::RButton;
				else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
					button = KeyCode::MButton;

				if ((wParam & MK_LBUTTON) != 0)
					state = KeyState::Press;

				if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_XBUTTONDOWN)
					state = KeyState::Press;
				else
					state = KeyState::Release;

				ReleaseCapture();
				if (state == KeyState::Press)
					SetCapture(hwnd);

				auto* thisObj = _getThis(hwnd);
				thisObj->m_windowData.windowBasicData.mouseButtons[static_cast<int>(button)] = static_cast<int>(state);
				thisObj->_onMouseButtonCallback(static_cast<int>(button), state);

			}break;

			case WM_MOUSEMOVE:
			{
				float x = static_cast<float>(GET_X_LPARAM(lParam));
				float y = static_cast<float>(GET_Y_LPARAM(lParam));
				auto* thisObj = _getThis(hwnd);
				thisObj->m_windowData.windowBasicData.mousePosition = {x, y};
				thisObj->_onMousePositionCallback(x, y);
			}break;

			case WM_MOUSEWHEEL:	 
			{
				const float x = 0.0f;
				const float y = -((SHORT)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA);

				auto* thisObj = _getThis(hwnd);
				thisObj->_onMouseScrollCallback(0.0, y);
			} break;

			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				int keycode = static_cast<UINT>(wParam);
				KeyState state = ((HIWORD(lParam) & KF_UP) == KF_UP) ? KeyState::Release : KeyState::Press;
				int repeatCount = static_cast<int>(LOWORD(lParam));

				// check if is repeat
				if ((lParam & (0x1 << 30)) && state == KeyState::Press)
					state = KeyState::Repeat;

				auto* thisObj = _getThis(hwnd);
				thisObj->m_windowData.windowBasicData.keys[keycode] = static_cast<int>(state);
				thisObj->_onKeyCallback(keycode, state, repeatCount);
			}break;

			case WM_CHAR:
			case WM_SYSCHAR:
			case WM_UNICHAR:
			{
				auto* thisObj = _getThis(hwnd);
				thisObj->_onCharCallback((unsigned int)wParam);

			} break;
			
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	void WindowsWindow::_pollEvents()
	{
		MSG msg;
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
				//VOG_CORE_LOG_TRACE(msg.message);
			}
		}
	}

	WindowsWindow* WindowsWindow::_getThis(HWND hwnd) {
		auto data = GetWindowLongPtr(hwnd, GWLP_USERDATA);
		auto* thisObj = reinterpret_cast<WindowsWindow*>(data);
		if (hwnd != thisObj->m_windowData.hwnd)
		{
			VOG_CORE_LOG_ERROR("Cannot get WindowsWindow* !");
		}
		return thisObj;
	}

	void WindowsWindow::_callAppOnEventCallBackFn(Event& event_)
	{
		if (m_windowData.appOnEventCallBackFn)
			m_windowData.appOnEventCallBackFn(event_);
	}

#pragma region WindowCallback

	void WindowsWindow::_onResizeCallback(uint32_t width_, uint32_t height_)
	{
		m_windowData.windowBasicData.width = width_;
		m_windowData.windowBasicData.height = height_;
		WindowResizeEvent event(width_, height_);
		_callAppOnEventCallBackFn(event);
	}

	void WindowsWindow::_onCloseCallback()
	{
		WindowCloseEvent event;
		_callAppOnEventCallBackFn(event);
	}

	void WindowsWindow::_onMouseScrollCallback(float xOffset_, float yOffset_)
	{
		//MouseScrolledEvent event(static_cast<float>(xOffset_), static_cast<float>(yOffset_));
		MouseScrolledEvent event(xOffset_, yOffset_);
		_callAppOnEventCallBackFn(event);
	}

	void WindowsWindow::_onMousePositionCallback(float xPos_, float yPos_)
	{
		//MouseMovedEvent event(static_cast<float>(xPos_), static_cast<float>(yPos_));
		MouseMovedEvent event(xPos_, yPos_);
		_callAppOnEventCallBackFn(event);
	}

	void WindowsWindow::_onMouseButtonCallback(int button_, KeyState state_)
	{
		switch (state_)
		{
			case KeyState::Release:
			{
				MouseButtonReleasedEvent event(button_);
				_callAppOnEventCallBackFn(event);
			} break;

			case KeyState::Press:
			{
				MouseButtonPressedEvent event(button_);
				_callAppOnEventCallBackFn(event);

			} break;
		}
	}

	void WindowsWindow::_onKeyCallback(int keycode_, KeyState state_, uint32_t repeatCount_)
	{
		switch (state_)
		{
			case KeyState::Release:
			{
				KeyReleasedEvent event(keycode_);
				_callAppOnEventCallBackFn(event);
			} break;

			case KeyState::Press:
			{
				KeyPressedEvent event(keycode_, 0);
				_callAppOnEventCallBackFn(event);

			} break;

			case KeyState::Repeat:
			{
				KeyPressedEvent event(keycode_, repeatCount_);
				_callAppOnEventCallBackFn(event);
			} break;
		}
	}

	void WindowsWindow::_onCharCallback(int keycode_)
	{
		KeyTypedEvent event(keycode_);
		_callAppOnEventCallBackFn(event);
	}

#pragma endregion

}