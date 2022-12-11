#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Vog/Physics/Body/BodyData.h"

#include "Vog/Physics/Collision/Shapes/AABBShape.h"
#include "Vog/Physics/Collision/Shapes/OBBShape.h"
#include "Vog/Physics/Collision/Shapes/TriangleShape.h"

namespace vog {

	struct SATInterval {
		float min;
		float max;

		static SATInterval getInterval(const AABBShape* pAABB_, const BodyTransform* pTransform_, const Vector3f& axis_);
		static SATInterval getInterval(const OBBShape* pOBB_, const BodyTransform* pTransform_, const Vector3f& axis_);
		static SATInterval getInterval(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const Vector3f& axis_);

		static bool isOverlapOnAxis(const AABBShape* pAABB_A_, const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_, const Vector3f& axis_);
		static bool isOverlapOnAxis(const OBBShape* pOBB_A_, const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_, const Vector3f& axis_);
		static bool isOverlapOnAxis(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_, const Vector3f& axis_);
		static bool isOverlapOnAxis(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_, const Vector3f& axis_);
		static bool isOverlapOnAxis(const Vector3f& pt0_A_, const Vector3f& pt1_A_, const Vector3f& pt2_A_, const Vector3f& pt0_B_, const Vector3f& pt1_B_, const Vector3f& pt2_B_, const Vector3f& axis_);
	};

}