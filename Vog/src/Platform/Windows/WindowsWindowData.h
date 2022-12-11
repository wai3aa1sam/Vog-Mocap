#pragma once

#include "Vog/Core/Core.h"
#include "Vog/Core/Window.h"

namespace vog {

	struct WindowData
	{
	public:
		WindowBasicData windowBasicData;
		bool isVsync = true;
		Window::AppOnEventCallBackFn appOnEventCallBackFn;

		HWND	hwnd = nullptr;
		HDC		hdc = nullptr;
	};
}