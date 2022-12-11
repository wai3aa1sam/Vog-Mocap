#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Vog/Physics/Body/Body.h"

namespace vog {

	struct RaycastResult
	{
		Body* pBody = nullptr;
		//float distance = -1.0f;				// ray origin to the impact point
		float t = 0.0f;
		Vector3f point = { 0.0f, 0.0f, 0.0f };
		Vector3f normal = { 0.0f, 0.0f, 0.0f };
	};
}