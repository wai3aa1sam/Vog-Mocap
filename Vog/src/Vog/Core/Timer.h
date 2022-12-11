#pragma once
#include "Vog/Core/Core.h"

namespace vog
{
	class VOG_API Timer
	{
	public:
		static float getTime();
		static float getMilliTime();

		static void reset();
	};
}