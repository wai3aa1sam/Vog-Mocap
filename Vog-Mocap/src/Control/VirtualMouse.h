#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

namespace vog {

	class VirtualMouse : public NonCopyable
	{
	public:
		void setMousePosition(int x_, int y_);
	private:
	};
}