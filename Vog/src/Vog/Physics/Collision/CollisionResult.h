#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Vog/Physics/Body/Body.h"

#include <vector>

namespace vog {

	struct CollisionConfig
	{
		bool isTrigger = false;
	};

	struct CollisionResult
	{
		operator bool() { return isCollided; }

		Body* pBody_A;
		Body* pBody_B;

		//Vector3f a;						// Furthest point of A into B
		//Vector3f b;						// Furthest point of B into A
		Vector3f direction;					// B - A normalized
		float penetration_depth;			// Length of B - A
		bool isCollided = false;

		Vector3f relative_velocity;			// relative to B
		std::vector<Vector3f> contact_points;

		CollisionConfig config;
	};
}