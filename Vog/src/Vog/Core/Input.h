#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Core/KeyCodes.h"

namespace vog {

	class VOG_API Input
	{
	public:
		static bool isKeyPressed(KeyCode keycode_);
		static bool isMousePressed(KeyCode button_);
		static std::pair<float, float> getMousePosition();
		static float getMousePositionX();
		static float getMousePositionY();
	};
}