#include "vogpch.h"
#include "Window.h"

#ifdef VOG_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif // DEBUG


namespace vog {

	ScopePtr<Window> Window::create(const WindowBasicData& windowBasicData_)
	{	
#ifdef VOG_PLATFORM_WINDOWS
		return createScopePtr<WindowsWindow>(windowBasicData_);
#else
		return nullptr;
#endif // DEBUG
	}
}