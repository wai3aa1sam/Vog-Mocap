#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

namespace vog {
	
	struct CollisionResult;

	namespace Utils {

		Vector3f getClosestPoint_Line(const Vector3f& ln_start_, const Vector3f& ln_end_, const Vector3f& point_);
		Vector3f getClosestPoint_Triangle(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const Vector3f& pt_);
		Vector3f getClosestPoint_AABB_By_Center(const Vector3f& center_, const Vector3f& size_, const Vector3f& point_);
		Vector3f getClosestPoint_AABB(const Vector3f& min_pt_, const Vector3f& max_pt_, const Vector3f& point_);
		Vector3f getClosestPoint_OBB(const Vector3f& center_, const Vector3f& size_, const Matrix3f& rotationMatrix_, const Vector3f& point_);
		Vector3f getClosestPoint_Plane(const Vector3f& normal_, float distance_, const Vector3f& point_);
		Vector3f getClosestPoint_Sphere(const Vector3f& center_, float radius_, const Vector3f& point_);

		bool isPointInTraingle(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const Vector3f& pt_);
		Vector3f getNormal_Triangle(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_);
		Vector3f getBarycentric(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const Vector3f& pt_);

		Vector3f project(const Vector3f& from_, const Vector3f& to_);
		Vector3f perpendicular(const Vector3f& from_, const Vector3f& to_);

		Vector3f getMinPoint_AABB(const Vector3f& center_, const Vector3f& size_);
		Vector3f getMaxPoint_AABB(const Vector3f& center_, const Vector3f& size_);

		Vector3f getCenter_AABB(const Vector3f& min_pt_, const Vector3f& max_pt_);
	}

}