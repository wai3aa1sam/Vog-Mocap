#include "VirtualMouse.h"
#include <conio.h>

namespace vog {

	void VirtualMouse::setMousePosition(int x_, int y_)
	{
		SetCursorPos(x_, y_);
	}
}