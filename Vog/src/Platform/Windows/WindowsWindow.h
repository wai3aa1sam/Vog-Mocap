#pragma once

#include "Vog/Core/Core.h"
#include "Vog/Core/Window.h"

#include "Vog/Graphics/GraphicsContext.h"

#include "WindowsWindowData.h"

#ifdef VOG_PLATFORM_WINDOWS

#include <Windows.h>
#include <Windowsx.h>
#include <conio.h>

#undef max
#undef min
#endif

namespace vog {

	class VOG_API WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowBasicData& windowBasicData_);
		virtual ~WindowsWindow();

		virtual void onUpdate() override;
		virtual void setAppOnEventCallBackFn(AppOnEventCallBackFn fn_) override;

		virtual inline uint32_t getWidth() const override;
		virtual inline uint32_t getHeight() const override;

		virtual void setVsync(bool isVsync_) override;
		virtual inline bool isVsync() const override;

		virtual inline void* getWindowHandle() const override { return m_windowData.hwnd; };
		virtual inline void* getGraphicsContext() const override { return m_pContext.get(); }

		inline const WindowData& getWindowData() const { return m_windowData; }

	private:
		void _init(const WindowBasicData& windowBasicData_);
		void _shutDown();

		void _onResizeCallback(uint32_t width_, uint32_t height_);
		void _onCloseCallback();
		void _onMouseScrollCallback(float xOffset_, float yOffset_);
		void _onMouseButtonCallback(int button_, KeyState state_);
		void _onMousePositionCallback(float xPos_, float yPos_);
		void _onKeyCallback(int keycode_, KeyState state_, uint32_t repeatCount_);
		void _onCharCallback(int keycode_);

		void _callAppOnEventCallBackFn(Event& event_);


		static LRESULT WINAPI _wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static WindowsWindow* _getThis(HWND hwnd);

		static void _pollEvents();

	protected:
		WindowData m_windowData;
	private:
		ScopePtr<GraphicsContext> m_pContext;
	};
}
