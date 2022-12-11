#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

namespace vog {

	struct VOG_API Ray /*: public NonCopyable*/
	{
	public:
		Ray(const Vector3f origin_, const Vector3f direction_)
			:
			origin(origin_), direction(direction_)
		{
		}
		Vector3f origin = { 0.0f, 0.0f, 0.0f };
		Vector3f direction = { 0.0f, 0.0f, 1.0f };
	private:
	};
}