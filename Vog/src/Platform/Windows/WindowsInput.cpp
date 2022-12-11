#include "vogpch.h"
#include "Vog/Core/Input.h"
#include "Vog/Core/Application.h"

#include "Platform/Windows/WindowsWindow.h"

namespace vog {
	static const WindowBasicData& _getWindowBasicData() { return static_cast<WindowsWindow*>(&Application::get().getWindow())->getWindowData().windowBasicData; }

	bool Input::isKeyPressed(KeyCode keycode_)
	{
		int state = _getWindowBasicData().keys[static_cast<int>(keycode_)];
		return state == KeyState::Press || state == KeyState::Repeat;
	}

	bool Input::isMousePressed(KeyCode button_)
	{
		int state = _getWindowBasicData().mouseButtons[static_cast<int>(button_)];
		return state == KeyState::Press;;
	}

	std::pair<float, float> Input::getMousePosition()
	{
		return _getWindowBasicData().mousePosition;
	}
	float Input::getMousePositionX()
	{
		return getMousePosition().first;
	}
	float Input::getMousePositionY()
	{
		return getMousePosition().second;
	}
}
