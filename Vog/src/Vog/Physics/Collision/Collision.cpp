#include "vogpch.h"
#include "Collision.h"

#include "CollisionResult.h"

#include "Vog/Physics/Body/BodyTransform.h"

#include "Vog/Physics/Collision/Bounding/SATInterval.h"

#include "Vog/Physics/Collision/Shapes/AABBShape.h"
#include "Vog/Physics/Collision/Shapes/CapsuleShape.h"
#include "Vog/Physics/Collision/Shapes/SphereShape.h"
#include "Vog/Physics/Collision/Shapes/OBBShape.h"
#include "Vog/Physics/Collision/Shapes/PlaneShape.h"
#include "Vog/Physics/Collision/Shapes/TriangleShape.h"

#include "Vog/Physics/Core/PhysicsConstant.h"

#include "Vog/Physics/Utils/Geometry3D.h"

#include "Vog/Debug/BenchmarkTimer.h"

// all SAT cross should check 0, other wise the parrallel axis will return 0, the check may wrong;

//checkSphereWithSphere			  correct						  
//checkSphereWithAABB			  wrong, correct now, wrong reason: get min / max, wrong size
//checkSphereWithPlane			  correct
//checkSphereWithOBB			  wrong, correct now, wrong reason: the closet point should start from center of obb
//								  
//checkAABBWithAABB				  wrong, correct now, wrong reason: copy problem, only used get min
//checkAABBWithOBB				  wrong, correct now, wrong reason: copy problem, want vertices
//checkAABBWithPlane			  wrong, still collide below the plane, correct now, wrong reason: the distance to plane should be abs!
//								  
//checkOBBWithOBB				  correct
//checkOBBWithPlane				  wrong, still collide below the plane, correct now, wrong reason: the distance to plane should be abs!
//								  
//checkPlaneWithPlane			  na
//checkShpereWithPoint			  na
//checkAABBWithPoint			  na
//checkOBBWithPoint				  na

namespace vog {
	
	CollisionResult Collision::checkAABBWithAABB(const AABBShape* pAABB_A_, const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_)
	{
		auto min_A = pAABB_A_->getMin(pTransform_A_->position);
		auto max_A = pAABB_A_->getMax(pTransform_A_->position);

		auto min_B = pAABB_B_->getMin(pTransform_B_->position);
		auto max_B = pAABB_B_->getMax(pTransform_B_->position);

		return checkAABBWithAABB(min_A, max_A, min_B, max_B);
	}

	CollisionResult Collision::checkAABBWithOBB(const AABBShape* pAABB_A_, const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_)
	{
		//axis
		std::array<Vector3f, 15> axes;
		
		axes[0] = Vector3f{1.0f, 0.0f, 0.0f};
		axes[1] = Vector3f{0.0f, 1.0f, 0.0f};
		axes[2] = Vector3f{0.0f, 0.0f, 1.0f};
		axes[3] = Vector3f{ pOBB_B_->m_rotationMatrix[0][0], pOBB_B_->m_rotationMatrix[0][1], pOBB_B_->m_rotationMatrix[0][2] };	// X axis of local obb
		axes[4] = Vector3f{ pOBB_B_->m_rotationMatrix[1][0], pOBB_B_->m_rotationMatrix[1][1], pOBB_B_->m_rotationMatrix[1][2] };	// Y axis of local obb
		axes[5] = Vector3f{ pOBB_B_->m_rotationMatrix[2][0], pOBB_B_->m_rotationMatrix[2][1], pOBB_B_->m_rotationMatrix[2][2] };	// Z axis of local obb

		for (size_t i = 0; i < 3; i++)
		{
			axes[6 + i * 3 + 0] = MyMath::cross(axes[i], axes[3]);
			axes[6 + i * 3 + 1] = MyMath::cross(axes[i], axes[4]);
			axes[6 + i * 3 + 2] = MyMath::cross(axes[i], axes[5]);
		}

		// test projection on each axis, if no overlap on one axis, no collision
		for (size_t i = 0; i < axes.size(); i++)
		{
			if (!SATInterval::isOverlapOnAxis(pAABB_A_, pTransform_A_, pOBB_B_, pTransform_B_, axes[i]))
				return CollisionResult{};
		}

		CollisionResult ret;

		ret.isCollided = true;
		//VOG_CORE_LOG_CRITICAL("no collision data!");
		//ret.direction = pOBB_B_->

		return ret;
	}

	CollisionResult Collision::checkAABBWithPoint(const AABBShape* pAABB_A_, const BodyTransform* pTransform_A_, const Vector3f& pt_B_)
	{
		CollisionResult ret;

		Vector3f min_pt = pAABB_A_->getMin(pTransform_A_->position);
		Vector3f max_pt = pAABB_A_->getMax(pTransform_A_->position);

		// check if the point is outside the aabb
		if (pt_B_.x < min_pt.x || pt_B_.y < min_pt.y || pt_B_.z < min_pt.z)
			return {};

		if (pt_B_.x > max_pt.x || pt_B_.y > max_pt.y || pt_B_.z > max_pt.z)
			return {};

		ret.isCollided = true;

		Vector3f center = pAABB_A_->m_center + pTransform_A_->position;
		Vector3f centerToPoint = pt_B_ - center;

		ret.penetration_depth = MyMath::magnitude(centerToPoint);

		if (ret.penetration_depth > 0.0f)
			ret.direction = centerToPoint / ret.penetration_depth;

		return ret;
	}

	CollisionResult Collision::checkAABBWithPlane(const AABBShape* pAABB_A_, const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_)
	{
		CollisionResult ret;
		
		auto center_aabb = pAABB_A_->m_center + pTransform_A_->position;

		//float aabb_size = MyMath::dot(pAABB_A_->m_size, MyMath::abs(pPlane_B_->m_normal));		// abs make the plane detect no mater the rotation is -ve
		float aabb_size = MyMath::abs(MyMath::dot(pAABB_A_->m_size, pPlane_B_->m_normal));			// abs make the plane detect no mater the rotation is -ve

		float distanceToAABB = MyMath::absf(MyMath::dot(center_aabb, pPlane_B_->m_normal) - pPlane_B_->m_distance);

		ret.isCollided = distanceToAABB <= aabb_size;
		if (ret.isCollided)
		{
			ret.penetration_depth = distanceToAABB - aabb_size;
			ret.direction = -pPlane_B_->m_normal;
		}

		return ret;
	}

	CollisionResult Collision::checkOBBWithOBB(const OBBShape* pOBB_A_, const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_)
	{
		//axis
		std::array<Vector3f, 15> axes;

		axes[0] = Vector3f{ pOBB_A_->m_rotationMatrix[0][0], pOBB_A_->m_rotationMatrix[0][1], pOBB_A_->m_rotationMatrix[0][2] };
		axes[1] = Vector3f{ pOBB_A_->m_rotationMatrix[1][0], pOBB_A_->m_rotationMatrix[1][1], pOBB_A_->m_rotationMatrix[1][2] };
		axes[2] = Vector3f{ pOBB_A_->m_rotationMatrix[2][0], pOBB_A_->m_rotationMatrix[2][1], pOBB_A_->m_rotationMatrix[2][2] };
		axes[3] = Vector3f{ pOBB_B_->m_rotationMatrix[0][0], pOBB_B_->m_rotationMatrix[0][1], pOBB_B_->m_rotationMatrix[0][2] };
		axes[4] = Vector3f{ pOBB_B_->m_rotationMatrix[1][0], pOBB_B_->m_rotationMatrix[1][1], pOBB_B_->m_rotationMatrix[1][2] };
		axes[5] = Vector3f{ pOBB_B_->m_rotationMatrix[2][0], pOBB_B_->m_rotationMatrix[2][1], pOBB_B_->m_rotationMatrix[2][2] };

		for (size_t i = 0; i < 3; i++)
		{
			axes[6 + i * 3 + 0] = MyMath::cross(axes[i], axes[3]);
			axes[6 + i * 3 + 1] = MyMath::cross(axes[i], axes[4]);
			axes[6 + i * 3 + 2] = MyMath::cross(axes[i], axes[5]);
		}

		// test projection on each axis, if no overlap on one axis, no collision
		for (size_t i = 0; i < axes.size(); i++)
		{
			if (!SATInterval::isOverlapOnAxis(pOBB_A_, pTransform_A_, pOBB_B_, pTransform_B_, axes[i]))
				return CollisionResult{};
		}

		CollisionResult ret;

		ret.isCollided = true;
		//VOG_CORE_LOG_CRITICAL("no collision data!");
		//ret.direction = pOBB_B_->

		return ret;
	}
	CollisionResult Collision::checkOBBWithPlane(const OBBShape* pOBB_A_, const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_)
	{
		CollisionResult ret;

		auto center_obb = pOBB_A_->m_center + pTransform_A_->position;

		std::array<Vector3f, 3> local_axis;

		local_axis[0] = Vector3f{ pOBB_A_->m_rotationMatrix[0][0], pOBB_A_->m_rotationMatrix[0][1], pOBB_A_->m_rotationMatrix[0][2] };
		local_axis[1] = Vector3f{ pOBB_A_->m_rotationMatrix[1][0], pOBB_A_->m_rotationMatrix[1][1], pOBB_A_->m_rotationMatrix[1][2] };
		local_axis[2] = Vector3f{ pOBB_A_->m_rotationMatrix[2][0], pOBB_A_->m_rotationMatrix[2][1], pOBB_A_->m_rotationMatrix[2][2] };

		// calculate project each rotation axis to plane normal
		float obb_size = pOBB_A_->m_size.x * MyMath::absf(MyMath::dot(pPlane_B_->m_normal, local_axis[0]))
						+ pOBB_A_->m_size.y * MyMath::absf(MyMath::dot(pPlane_B_->m_normal, local_axis[1]))
						+ pOBB_A_->m_size.z * MyMath::absf(MyMath::dot(pPlane_B_->m_normal, local_axis[2]));

		float distanceToPlane = MyMath::abs(MyMath::dot(center_obb, pPlane_B_->m_normal) - pPlane_B_->m_distance);

		ret.isCollided = distanceToPlane <= obb_size;
		if (ret.isCollided)
		{
			ret.penetration_depth = obb_size - distanceToPlane;
			ret.direction = -pPlane_B_->m_normal;
		}

		return ret;
	}
	CollisionResult Collision::checkPlaneWithPlane(const PlaneShape* pPlane_A_, const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_)
	{
		CollisionResult ret;

		auto normal_a = pPlane_A_->m_normal;
		auto normal_b = pPlane_B_->m_normal;
		
		auto cross = MyMath::cross(normal_a, normal_b);
		
		ret.isCollided = !MyMath::isEqualTo(MyMath::dot(cross, cross), 0.0f);

		if (ret.isCollided)
		{
			//VOG_CORE_LOG_CRITICAL("no collision data!");

		}
		return ret;
	}

#pragma region Sphere_collision
	CollisionResult Collision::checkSphereWithSphere(const SphereShape* pSphere_A_, const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_)
	{
		auto center_A = pSphere_A_->m_center + pTransform_A_->position;
		auto center_B = pSphere_B_->m_center + pTransform_B_->position;

		auto radius_A = pSphere_A_->m_radius;
		auto radius_B = pSphere_B_->m_radius;

		return checkSphereWithSphere(center_A, radius_A, center_B, radius_B);
	}
	CollisionResult Collision::checkSphereWithPlane(const SphereShape* pSphere_A_, const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_)
	{
		auto sphereCenter = pSphere_A_->m_center + pTransform_A_->position;
		auto radius = pSphere_A_->m_radius;

		auto plane_normal = pPlane_B_->m_normal;
		auto plane_distance = pPlane_B_->m_distance;

		/*auto plane_normal = MyMath::toMatrix3f(pTransform_B_->orientation) * Vector3f(0.0f, 1.0f, 0.0f);
		plane_normal = MyMath::normalize(plane_normal);
		float plane_distance = MyMath::magnitude(pTransform_B_->position);
		if (pTransform_B_->position.y < 0.0f)
			plane_distance *= -1.0f;*/

		return checkSphereWithPlane(sphereCenter, radius, plane_normal, plane_distance);
	}
	CollisionResult Collision::checkSphereWithAABB(const SphereShape* pSphere_A_, const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_)
	{
		auto sphereCenter = pSphere_A_->m_center + pTransform_A_->position;
		auto sphereRadius = pSphere_A_->m_radius;

		auto aabbCenter = pAABB_B_->m_center + pTransform_B_->position;
		auto aabbSize = pAABB_B_->m_size;

		return checkSphereWithAABB(sphereCenter, sphereRadius, Utils::getMinPoint_AABB(aabbCenter, aabbSize), Utils::getMaxPoint_AABB(aabbCenter, aabbSize));
	}
	CollisionResult Collision::checkSphereWithOBB(const SphereShape* pSphere_A_, const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_)
	{
		auto sphereCenter = pSphere_A_->m_center + pTransform_A_->position;
		auto sphereRadius = pSphere_A_->m_radius;

		auto obbCenter = pOBB_B_->m_center + pTransform_B_->position;
		auto obbSize = pOBB_B_->m_size;
		auto& obbRotationMatrix = pOBB_B_->m_rotationMatrix;
		return checkSphereWithOBB(sphereCenter, sphereRadius, obbCenter, obbSize, obbRotationMatrix);
	}

	CollisionResult Collision::checkSphereWithSphere(const Vector3f& center_A_, float radius_A_, const Vector3f& center_B_, float radius_B_)
	{
		//BenchmarkTimer timer("checkSphereWithSphere");
		CollisionResult ret;

		auto center_A = center_A_;
		auto center_B = center_B_;

		auto radius_A = radius_A_;
		auto radius_B = radius_B_;

		Vector3f direction = center_B - center_A;
		float center_dist_sq = MyMath::magnitude_square(direction);
		float radius_dist_sq = (radius_A + radius_B) * (radius_A + radius_B);

		ret.isCollided = center_dist_sq < radius_dist_sq;
		if (ret.isCollided)
		{
			float center_dist = MyMath::sqrt(center_dist_sq);
			ret.penetration_depth = (radius_A + radius_B - center_dist) * 0.5f;

			if (center_dist > 0.0f)
				ret.direction = direction / center_dist;
			else
				ret.direction = direction;

			Vector3f contact_point = center_A + direction * (radius_A - ret.penetration_depth);
			ret.contact_points.push_back(contact_point);
		}
		return ret;
	}
	CollisionResult Collision::checkSphereWithAABB(const Vector3f& center_A_, float radius_A_, const Vector3f& min_pt_B_, const Vector3f& max_pt_B_)
	{
		CollisionResult ret;

		auto sphereCenter = center_A_;
		auto sphereRadius = radius_A_;

		Vector3f closest_pt = Utils::getClosestPoint_AABB(min_pt_B_, max_pt_B_, sphereCenter);

		auto closest_pt_ToCenter = sphereCenter - closest_pt;

		auto dist_sq = MyMath::magnitude_square(closest_pt_ToCenter);

		ret.isCollided = dist_sq < sphereRadius * sphereRadius;
		if (ret.isCollided)
		{
			float dist = MyMath::sqrt(dist_sq);
			ret.penetration_depth = sphereRadius - dist;
			if (dist > 0.0f)
				ret.direction = closest_pt_ToCenter / dist;
		}

		return ret;
	}
	CollisionResult Collision::checkSphereWithOBB(const Vector3f& center_A_, float radius_A_, const Vector3f& center_B_, const Vector3f& size_B_, const Matrix3f& rotationMatrix_B_)
	{
		CollisionResult ret;

		auto sphereCenter = center_A_;
		auto sphereRadius = radius_A_;

		Vector3f closest_pt = Utils::getClosestPoint_OBB(center_B_, size_B_, rotationMatrix_B_, sphereCenter);
		auto center_to_closest_pt_ = closest_pt - sphereCenter;

		auto dist_sq = MyMath::magnitude_square(center_to_closest_pt_);

		ret.isCollided = dist_sq < sphereRadius * sphereRadius;
		if (!ret.isCollided)
			return ret;

		float dist = MyMath::sqrt(dist_sq);
		ret.penetration_depth = (sphereRadius - dist) * 0.5f;

		if (dist > 0.0f)
			ret.direction = center_to_closest_pt_ / dist;
		else
			ret.direction = center_to_closest_pt_;

		Vector3f contact_point = closest_pt + ret.direction * ret.penetration_depth;
		ret.contact_points.push_back(contact_point);

		return ret;
	}
	CollisionResult Collision::checkSphereWithPlane(const Vector3f& center_A_, float radius_A_, const Vector3f& normal_B_, float distance_B_)
	{
		CollisionResult ret;

		auto sphereCenter = center_A_;
		auto sphereRadius = radius_A_;

		Vector3f closetPoint = Utils::getClosestPoint_Plane(normal_B_, distance_B_, sphereCenter);

		float distanceFromSphere_sq = MyMath::magnitude_square(sphereCenter - closetPoint);

		ret.isCollided = distanceFromSphere_sq < sphereRadius * sphereRadius;
		if (ret.isCollided)
		{
			float distanceFromSphere = MyMath::sqrt(distanceFromSphere_sq);
			ret.penetration_depth = sphereRadius - distanceFromSphere;
			if (distanceFromSphere > 0.0f)
				ret.direction = (closetPoint - sphereCenter) / distanceFromSphere;
			else
				ret.direction = (closetPoint - sphereCenter);
		}

		return ret;
	}
	CollisionResult Collision::checkShpereWithPoint(const SphereShape* pSphere_A_, const BodyTransform* pTransform_A_, const Vector3f& pt_B_)
	{
		CollisionResult ret;

		auto sphereCenter = pSphere_A_->m_center + pTransform_A_->position;

		auto centerToPoint = pt_B_ - sphereCenter;
		auto magnitude_sq = MyMath::magnitude_square(centerToPoint);

		ret.isCollided = magnitude_sq < pSphere_A_->m_radius* pSphere_A_->m_radius;
		if (ret.isCollided)
		{
			ret.penetration_depth = MyMath::sqrt(magnitude_sq);
			ret.direction = MyMath::normalize(sphereCenter);
		}

		return ret;
	}
#pragma endregion

#pragma region AABB_collision
	CollisionResult Collision::checkAABBWithAABB(const Vector3f& min_A_, const Vector3f& max_A_, const Vector3f& min_B_, const Vector3f& max_B_)
	{
		CollisionResult ret;

		auto min_A = min_A_;
		auto max_A = max_A_;

		auto min_B = min_B_;
		auto max_B = max_B_;

		ret.isCollided = min_A.x <= max_B.x && max_A.x >= min_B.x
			&& min_A.y <= max_B.y && max_A.y >= min_B.y
			&& min_A.z <= max_B.z && max_A.z >= min_B.z;
		if (ret.isCollided)
		{
			Vector3f direction;
			if (min_A.x <= min_B.x)		// a in left, b in right
				direction = min_B - min_A;
			else
				direction = max_B - min_A;

			ret.penetration_depth = MyMath::magnitude(direction);
			if (ret.penetration_depth > 0.0f)
				ret.direction = direction / ret.penetration_depth;
		}

		return ret;
	}
#pragma endregion

#pragma region Triangle_collision
	CollisionResult Collision::checkTriangleWithSphere(const TriangleShape* pTriangle_A_, const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_)
	{
		Matrix4f transform = pTransform_A_->getTransform();
		auto pt0 = transform * Vector4f(pTriangle_A_->m_pt0, 1.0f);
		auto pt1 = transform * Vector4f(pTriangle_A_->m_pt1, 1.0f);
		auto pt2 = transform * Vector4f(pTriangle_A_->m_pt2, 1.0f);

		auto sphereCenter = pSphere_B_->m_center + pTransform_B_->position;
		auto sphereRadius = pSphere_B_->m_radius;
		return checkTriangleWithSphere(pt0, pt1, pt2, sphereCenter, sphereRadius);
	}
	CollisionResult Collision::checkTriangleWithPlane(const TriangleShape* pTriangle_A_, const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_)
	{
		Matrix4f transform = pTransform_A_->getTransform();
		auto pt0 = transform * Vector4f(pTriangle_A_->m_pt0, 1.0f);
		auto pt1 = transform * Vector4f(pTriangle_A_->m_pt1, 1.0f);
		auto pt2 = transform * Vector4f(pTriangle_A_->m_pt2, 1.0f);

		/*auto plane_normal = MyMath::toMatrix3f(pTransform_B_->orientation) * Vector3f(0.0f, 1.0f, 0.0f);
		plane_normal = MyMath::normalize(plane_normal);
		float plane_distance = MyMath::magnitude(pTransform_B_->position);
		if (pTransform_B_->position.y < 0.0f)
			plane_distance *= -1.0f;*/
		return checkTriangleWithPlane(pt0, pt1, pt2, pPlane_B_->m_normal, pPlane_B_->m_distance);
	}
	CollisionResult Collision::checkTriangleWithAABB(const TriangleShape* pTriangle_A_, const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_)
	{
		Matrix4f transform = pTransform_A_->getTransform();
		auto pt0 = transform * Vector4f(pTriangle_A_->m_pt0, 1.0f);
		auto pt1 = transform * Vector4f(pTriangle_A_->m_pt1, 1.0f);
		auto pt2 = transform * Vector4f(pTriangle_A_->m_pt2, 1.0f);
		return checkTriangleWithAABB(pt0, pt1, pt2, pAABB_B_, pTransform_B_);
	}
	CollisionResult Collision::checkTriangleWithOBB(const TriangleShape* pTriangle_A_, const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_)
	{
		Matrix4f transform = pTransform_A_->getTransform();
		auto pt0 = transform * Vector4f(pTriangle_A_->m_pt0, 1.0f);
		auto pt1 = transform * Vector4f(pTriangle_A_->m_pt1, 1.0f);
		auto pt2 = transform * Vector4f(pTriangle_A_->m_pt2, 1.0f);
		return checkTriangleWithOBB(pt0, pt1, pt2, pOBB_B_, pTransform_B_);
	}
	CollisionResult Collision::checkTriangleWithTriangle(const TriangleShape* pTriangle_A_, const BodyTransform* pTransform_A_, const TriangleShape* pTriangle_B_, const BodyTransform* pTransform_B_)
	{
		Matrix4f transform_A = pTransform_A_->getTransform();
		auto pt0_A = transform_A * Vector4f(pTriangle_A_->m_pt0, 1.0f);
		auto pt1_A = transform_A * Vector4f(pTriangle_A_->m_pt1, 1.0f);
		auto pt2_A = transform_A * Vector4f(pTriangle_A_->m_pt2, 1.0f);

		Matrix4f transform_B = pTransform_B_->getTransform();
		auto pt0_B = transform_B * Vector4f(pTriangle_B_->m_pt0, 1.0f);
		auto pt1_B = transform_B * Vector4f(pTriangle_B_->m_pt1, 1.0f);
		auto pt2_B = transform_B * Vector4f(pTriangle_B_->m_pt2, 1.0f);
		return checkTriangleWithTriangle(pt0_A, pt1_A, pt2_A, pt0_B, pt1_B, pt2_B);
	}

	CollisionResult Collision::checkTriangleWithSphere(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const Vector3f& center_B_, float radius_B_)
	{
		CollisionResult ret;

		auto sphereCenter = center_B_;
		auto sphereRadius = radius_B_;

		Vector3f closest_pt = Utils::getClosestPoint_Triangle(pt0_, pt1_, pt2_, sphereCenter);
		auto dir_to_center = sphereCenter - closest_pt;
		auto dist_to_center_sq = MyMath::magnitude_square(dir_to_center);

		ret.isCollided = dist_to_center_sq <= sphereRadius * sphereRadius;
		if (ret.isCollided)
		{
			float dist_to_center = MyMath::sqrt(dist_to_center_sq);
			ret.penetration_depth = sphereRadius - dist_to_center;
			ret.direction = dir_to_center;
		}

		return ret;
	}
	CollisionResult Collision::checkTriangleWithPlane(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const Vector3f& normal_B_, float distance_B_)
	{
		CollisionResult ret;

		float dist_to_plane0 = MyMath::dot(pt0_, normal_B_) - distance_B_;
		float dist_to_plane1 = MyMath::dot(pt1_, normal_B_) - distance_B_;
		float dist_to_plane2 = MyMath::dot(pt2_, normal_B_) - distance_B_;

		ret.isCollided = false;
		if ((dist_to_plane0 > 0.0f && dist_to_plane1 > 0.0f && dist_to_plane2 > 0.0f) || (dist_to_plane0 < 0.0f && dist_to_plane1 < 0.0f && dist_to_plane2 < 0.0f))		// above or below the plane
			return ret;

		ret.isCollided = true;

		if (MyMath::isEqualTo(dist_to_plane0, 0.0f) && MyMath::isEqualTo(dist_to_plane1, 0.0f) && MyMath::isEqualTo(dist_to_plane2, 0.0f))		// on plane
			return ret;

		return ret;
	}
	CollisionResult Collision::checkTriangleWithAABB(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_)
	{
		CollisionResult ret;

		Vector3f edge0 = pt1_ - pt0_;
		Vector3f edge1 = pt2_ - pt1_;
		Vector3f edge2 = pt0_ - pt2_;

		std::array<Vector3f, 13> axes;
		axes[0] = Vector3f{ 1.0f, 0.0f, 0.0f };
		axes[1] = Vector3f{ 0.0f, 1.0f, 0.0f };
		axes[2] = Vector3f{ 0.0f, 0.0f, 1.0f };
		axes[3] = MyMath::cross(edge0, edge1);			// normal of triangle
		axes[4] = MyMath::cross(edge0, axes[0]);		axes[5] = MyMath::cross(edge0, axes[1]);		axes[6] = MyMath::cross(edge0, axes[2]);
		axes[7] = MyMath::cross(edge1, axes[0]);		axes[8] = MyMath::cross(edge1, axes[1]);		axes[9] = MyMath::cross(edge1, axes[2]);
		axes[10] = MyMath::cross(edge2, axes[0]);		axes[11] = MyMath::cross(edge2, axes[1]);		axes[12] = MyMath::cross(edge2, axes[2]);

		for (size_t i = 0; i < axes.size(); i++)
		{
			if (!SATInterval::isOverlapOnAxis(pt0_, pt1_, pt2_, pAABB_B_, pTransform_B_, axes[i]))
				return {};
		}

		ret.isCollided = true;
		return ret;
	}
	CollisionResult Collision::checkTriangleWithOBB(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_)
	{
		CollisionResult ret;

		Vector3f edge0 = pt1_ - pt0_;
		Vector3f edge1 = pt2_ - pt1_;
		Vector3f edge2 = pt0_ - pt2_;

		std::array<Vector3f, 13> axes;
		axes[0] = Vector3f{ pOBB_B_->m_rotationMatrix[0][0], pOBB_B_->m_rotationMatrix[0][1], pOBB_B_->m_rotationMatrix[0][2] };
		axes[1] = Vector3f{ pOBB_B_->m_rotationMatrix[1][0], pOBB_B_->m_rotationMatrix[1][1], pOBB_B_->m_rotationMatrix[1][2] };
		axes[2] = Vector3f{ pOBB_B_->m_rotationMatrix[2][0], pOBB_B_->m_rotationMatrix[2][1], pOBB_B_->m_rotationMatrix[2][2] };
		axes[3] = MyMath::cross(edge0, edge1);			// normal of triangle
		axes[4] = MyMath::cross(edge0, axes[0]);		axes[5] = MyMath::cross(edge0, axes[1]);		axes[6] = MyMath::cross(edge0, axes[2]);
		axes[7] = MyMath::cross(edge1, axes[0]);		axes[8] = MyMath::cross(edge1, axes[1]);		axes[9] = MyMath::cross(edge1, axes[2]);
		axes[10] = MyMath::cross(edge2, axes[0]);		axes[11] = MyMath::cross(edge2, axes[1]);		axes[12] = MyMath::cross(edge2, axes[2]);

		for (size_t i = 0; i < axes.size(); i++)
		{
			if (!SATInterval::isOverlapOnAxis(pt0_, pt1_, pt2_, pOBB_B_, pTransform_B_, axes[i]))
				return {};
		}

		ret.isCollided = true;
		return ret;
	}
	CollisionResult Collision::checkTriangleWithTriangle(const Vector3f& pt0_A_, const Vector3f& pt1_A_, const Vector3f& pt2_A_, const Vector3f& pt0_B_, const Vector3f& pt1_B_, const Vector3f& pt2_B_)
	{
		CollisionResult ret;

		Vector3f edge0_A = pt1_A_ - pt0_A_;
		Vector3f edge1_A = pt2_A_ - pt1_A_;
		Vector3f edge2_A = pt0_A_ - pt2_A_;

		Vector3f edge0_B = pt1_B_ - pt0_B_;
		Vector3f edge1_B = pt2_B_ - pt1_B_;
		Vector3f edge2_B = pt0_B_ - pt2_B_;

		std::array<Vector3f, 11> axes;
		axes[0] = MyMath::cross(edge0_A, edge1_A);
		axes[1] = MyMath::cross(edge0_B, edge1_B);
		axes[2] = MyMath::cross(edge0_A, edge0_B);		axes[3] = MyMath::cross(edge0_A, edge1_B);		axes[4] = MyMath::cross(edge0_A, edge2_B);
		axes[5] = MyMath::cross(edge1_A, edge0_B);		axes[6] = MyMath::cross(edge1_A, edge1_B);		axes[7] = MyMath::cross(edge1_A, edge2_B);
		axes[8] = MyMath::cross(edge2_A, edge0_B);		axes[9] = MyMath::cross(edge2_A, edge1_B);		axes[10] = MyMath::cross(edge2_A, edge2_B);

		for (size_t i = 0; i < axes.size(); i++)
		{
			if (!SATInterval::isOverlapOnAxis(pt1_A_, pt1_A_, pt2_A_, pt0_B_, pt1_B_, pt2_B_, axes[i]))
				return {};
		}

		ret.isCollided = true;
		return ret;
	}

#pragma endregion

#pragma region Capsule_collision
	CollisionResult Collision::checkCapsuleWithAABB(const CapsuleShape* pCapsule_A_, const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_)
	{
		auto center_A = pTransform_A_->orientation * pCapsule_A_->m_center + pTransform_A_->position;

		auto center_aabb = pAABB_B_->m_center + pTransform_B_->position;
		auto min_pt_aabb = Utils::getMinPoint_AABB(center_aabb, pAABB_B_->m_size);
		auto max_pt_aabb = Utils::getMaxPoint_AABB(center_aabb, pAABB_B_->m_size);

		return checkCapsuleWithAABB(center_A, pCapsule_A_->m_normal, pCapsule_A_->m_height, pCapsule_A_->m_radius, 
										min_pt_aabb, max_pt_aabb);
	}
	CollisionResult Collision::checkCapsuleWithCapsule(const CapsuleShape* pCapsule_A_, const BodyTransform* pTransform_A_, const CapsuleShape* pCapsule_B_, const BodyTransform* pTransform_B_)
	{
		auto center_A = pTransform_A_->orientation * pCapsule_A_->m_center + pTransform_A_->position;
		auto center_B = pTransform_B_->orientation * pCapsule_B_->m_center + pTransform_B_->position;

		return checkCapsuleWithCapsule(center_A, pCapsule_A_->m_normal, pCapsule_A_->m_height, pCapsule_A_->m_radius, 
										center_B, pCapsule_B_->m_normal, pCapsule_B_->m_height, pCapsule_B_->m_radius);
	}

	CollisionResult Collision::checkCapsuleWithOBB(const CapsuleShape* pCapsule_A_, const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_)
	{
		auto center_A = pTransform_A_->orientation * pCapsule_A_->m_center + pTransform_A_->position;

		auto obbCenter = pOBB_B_->m_center + pTransform_B_->position;
		auto obbSize = pOBB_B_->m_size;
		auto& obbRotationMatrix = pOBB_B_->m_rotationMatrix;

		return checkCapsuleWithOBB(center_A, pCapsule_A_->m_normal, pCapsule_A_->m_height, pCapsule_A_->m_radius, 
									obbCenter, obbSize, obbRotationMatrix);
	}

	CollisionResult Collision::checkCapsuleWithPlane(const CapsuleShape* pCapsule_A_, const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_)
	{
		auto center_A = pTransform_A_->orientation * pCapsule_A_->m_center + pTransform_A_->position;

		auto plane_normal = pPlane_B_->m_normal;
		auto plane_distance = pPlane_B_->m_distance;
		return checkCapsuleWithPlane(center_A, pCapsule_A_->m_normal, pCapsule_A_->m_height, pCapsule_A_->m_radius, 
										plane_normal, plane_distance);
	}

	CollisionResult Collision::checkCapsuleWithSphere(const CapsuleShape* pCapsule_A_, const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_)
	{
		auto center_A = pTransform_A_->orientation * pCapsule_A_->m_center + pTransform_A_->position;

		auto sphereCenter = pSphere_B_->m_center + pTransform_B_->position;
		auto sphereRadius = pSphere_B_->m_radius;
		return checkCapsuleWithSphere(center_A, pCapsule_A_->m_normal, pCapsule_A_->m_height, pCapsule_A_->m_radius, 
										sphereCenter, sphereRadius);
	}

	CollisionResult Collision::checkCapsuleWithTriangle(const CapsuleShape* pCapsule_A_, const BodyTransform* pTransform_A_, const TriangleShape* pTriangle_B_, const BodyTransform* pTransform_B_)
	{
		auto center_A = pTransform_A_->orientation * pCapsule_A_->m_center + pTransform_A_->position;

		Matrix4f transform = pTransform_B_->getTransform();
		auto pt0 = transform * Vector4f(pTriangle_B_->m_pt0, 1.0f);
		auto pt1 = transform * Vector4f(pTriangle_B_->m_pt1, 1.0f);
		auto pt2 = transform * Vector4f(pTriangle_B_->m_pt2, 1.0f);

		return checkCapsuleWithTriangle(center_A, pCapsule_A_->m_normal, pCapsule_A_->m_height, pCapsule_A_->m_radius,
											pt0, pt1, pt2);
	}

	// ========================Implementation===========================
	CollisionResult Collision::checkCapsuleWithAABB(const Vector3f& center_A_, const Vector3f& normal_A_, float height_A_, float radius_A_, const Vector3f& min_pt_B_, const Vector3f& max_pt_B_)
	{
		auto offset_A = normal_A_ * (height_A_ / 2.0f - radius_A_);
		auto base_center_A = center_A_ - offset_A;
		auto tip_center_A = center_A_ + offset_A;

		auto center_aabb = Utils::getCenter_AABB(min_pt_B_, max_pt_B_);

		Vector3f best_center = Utils::getClosestPoint_Line(base_center_A, tip_center_A, center_aabb);

		CollisionResult ret = checkSphereWithAABB(best_center, radius_A_, min_pt_B_, max_pt_B_);
		return ret;
	}

	CollisionResult Collision::checkCapsuleWithCapsule(const Vector3f& center_A_, const Vector3f& normal_A_, float height_A_, float radius_A_, const Vector3f& center_B_, const Vector3f& normal_B_, float height_B_, float radius_B_)
	{
		auto offset_A = normal_A_ * (height_A_ / 2.0f - radius_A_);
		auto base_center_A = center_A_ - offset_A;
		auto tip_center_A = center_A_ + offset_A;

		auto offset_B = normal_B_ * (height_B_ / 2.0f - radius_B_);
		auto base_center_B = center_B_ - offset_B;
		auto tip_center_B = center_B_  + offset_B;

		auto dir_base_B_base_A	= base_center_B - base_center_A;
		auto dir_tip_B_base_A	= tip_center_B - base_center_A;
		auto dir_base_B_tip_A	= base_center_B - tip_center_A;
		auto dir_tip_B_tip_A	= tip_center_B - tip_center_A;

		auto dir_base_B_base_A_mag_sq	= MyMath::dot(dir_base_B_base_A, dir_base_B_base_A);
		auto dir_tip_B_base_A_mag_sq	= MyMath::dot(dir_tip_B_base_A, dir_tip_B_base_A);
		auto dir_base_B_tip_A_mag_sq	= MyMath::dot(dir_base_B_tip_A, dir_base_B_tip_A);
		auto dir_tip_B_tip_A_mag_sq		= MyMath::dot(dir_tip_B_tip_A, dir_tip_B_tip_A);

		//VOG_CORE_LOG_INFO("d0: {0}, d1: {1}, d2: {2}, d3: {3}", dir_base_B_base_A_mag_sq, dir_tip_B_base_A_mag_sq, dir_base_B_tip_A_mag_sq, dir_tip_B_tip_A_mag_sq);

		Vector3f best_center_A;
		Vector3f best_center_B; 
		
		if (dir_base_B_base_A_mag_sq > dir_base_B_tip_A_mag_sq || dir_tip_B_base_A_mag_sq > dir_base_B_tip_A_mag_sq
			|| dir_base_B_base_A_mag_sq > dir_tip_B_tip_A_mag_sq || dir_tip_B_base_A_mag_sq > dir_tip_B_tip_A_mag_sq)
		{
			best_center_A = tip_center_A;
		}
		else
			best_center_A = base_center_A;

		best_center_B = Utils::getClosestPoint_Line(base_center_B, tip_center_B, best_center_A);
		best_center_A = Utils::getClosestPoint_Line(base_center_A, tip_center_A, best_center_B);

		CollisionResult ret;

		ret = checkSphereWithSphere(best_center_A, radius_A_, best_center_B, radius_B_);

		if (ret.isCollided)
		{
			//VOG_CORE_LOG_INFO("Capsule Collided With Capsule!");

			//VOG_CORE_LOG_INFO("A base center: <{0}, {1}, {2}>, A tip center: <{3}, {4}, {5}>, A normal: <{6}, {7}, {8}>", base_center_A.x, base_center_A.y, base_center_A.z,
			//	tip_center_A.x, tip_center_A.y, tip_center_A.z, normal_A_.x, normal_A_.y, normal_A_.z);
		}

		return ret;
	}

	CollisionResult Collision::checkCapsuleWithOBB(const Vector3f& center_A_, const Vector3f& normal_A_, float height_A_, float radius_A_, const Vector3f& center_B_, const Vector3f& size_B_, const Matrix3f& rotationMatrix_B_)
	{
		auto offset_A = normal_A_ * (height_A_ / 2.0f - radius_A_);
		auto base_center_A = center_A_ - offset_A;
		auto tip_center_A = center_A_ + offset_A;

		Vector3f best_center = Utils::getClosestPoint_Line(base_center_A, tip_center_A, center_B_);

		CollisionResult ret = checkSphereWithOBB(best_center, radius_A_, center_B_, size_B_, rotationMatrix_B_);

		//VOG_CORE_LOG_INFO("A base center: <{0}, {1}, {2}>, A tip center: <{3}, {4}, {5}>, A best_center: <{6}, {7}, {8}>", base_center_A.x, base_center_A.y, base_center_A.z,
		//	tip_center_A.x, tip_center_A.y, tip_center_A.z, best_center.x, best_center.y, best_center.z);

		/*if (ret.isCollided)
		{
			VOG_CORE_LOG_INFO("Capsule Collided With OBB!");
		}*/

		return ret;
	}

	CollisionResult Collision::checkCapsuleWithPlane(const Vector3f& center_A_, const Vector3f& normal_A_, float height_A_, float radius_A_, const Vector3f& normal_B_, float distance_B_)
	{
		auto offset_A = normal_A_ * (height_A_ / 2.0f - radius_A_);
		auto base_center_A = center_A_ - offset_A;
		auto tip_center_A = center_A_ + offset_A;
		auto dist_tip_bsae = height_A_ - 2.0f * radius_A_;

		// find t of line with plane
		auto proj_dir = MyMath::dot(normal_B_, normal_A_);
		auto proj_dist = MyMath::dot(normal_B_, base_center_A);

		float t = (distance_B_ - proj_dist) / proj_dir;

		Vector3f best_center;

		if (t <= 0.0f)
			best_center = base_center_A;
		else if (t >= 1.0f)
			best_center = tip_center_A;
		else
			best_center = base_center_A + normal_A_ * dist_tip_bsae * t;

		CollisionResult ret = checkSphereWithPlane(best_center, radius_A_, normal_B_, distance_B_);

		return ret;
	}

	CollisionResult Collision::checkCapsuleWithSphere(const Vector3f& center_A_, const Vector3f& normal_A_, float height_A_, float radius_A_, const Vector3f& center_B_, float radius_B_)
	{
		auto offset_A = normal_A_ * (height_A_ / 2.0f - radius_A_);
		auto base_center_A = center_A_ - offset_A;
		auto tip_center_A = center_A_ + offset_A;

		Vector3f best_center = Utils::getClosestPoint_Line(base_center_A, tip_center_A, center_B_);

		CollisionResult ret = checkSphereWithSphere(best_center, radius_A_, center_B_, radius_B_);
		return ret;
	}

	CollisionResult Collision::checkCapsuleWithTriangle(const Vector3f& center_A_, const Vector3f& normal_A_, float height_A_, float radius_A_, const Vector3f& pt0_B_, const Vector3f& pt1_B_, const Vector3f& pt2_B_)
	{
		BenchmarkTimer timer("checkCapsuleWithTriangle");
		auto offset_A = normal_A_ * (height_A_ / 2.0f - radius_A_);
		auto base_center_A = center_A_ - offset_A;
		auto tip_center_A = center_A_ + offset_A;

		//VOG_CORE_LOG_INFO("A base center: <{0}, {1}, {2}>, A tip center: <{3}, {4}, {5}>, A normal: <{6}, {7}, {8}>", base_center_A.x, base_center_A.y, base_center_A.z,
		//	tip_center_A.x, tip_center_A.y, tip_center_A.z, normal_A_.x, normal_A_.y, normal_A_.z);

		{	// simple wicked engine method
			auto triangle_normal = Utils::getNormal_Triangle(pt0_B_, pt1_B_, pt2_B_);
			Vector3f line_plane_intersection = Utils::getClosestPoint_Plane(triangle_normal, MyMath::magnitude(pt0_B_), base_center_A);
			Vector3f reference_pt = Utils::getClosestPoint_Triangle(pt0_B_, pt1_B_, pt2_B_, line_plane_intersection);
			Vector3f best_center = Utils::getClosestPoint_Line(base_center_A, tip_center_A, reference_pt);

			return checkTriangleWithSphere(pt0_B_, pt1_B_, pt2_B_, best_center, radius_A_);
		}
		
		{	// from wicked engine
			auto base_A = center_A_ - normal_A_ * (height_A_ / 2.0f);

			auto triangle_normal = Utils::getNormal_Triangle(pt0_B_, pt1_B_, pt2_B_);

			float t = MyMath::dot(triangle_normal, (pt0_B_ - base_A) / MyMath::abs(MyMath::dot(triangle_normal, normal_A_)));
			Vector3f line_plane_intersection = base_A + normal_A_ * t;
			Vector3f reference_pt = Utils::getClosestPoint_Triangle(pt0_B_, pt1_B_, pt2_B_, line_plane_intersection);

			Vector3f best_center = Utils::getClosestPoint_Line(base_center_A, tip_center_A, reference_pt);

			bool is_inside = Utils::isPointInTraingle(pt0_B_, pt1_B_, pt2_B_, line_plane_intersection);

			if (is_inside)
			{
				reference_pt = line_plane_intersection;
			}
			else
			{
				Vector3f pt1 = Utils::getClosestPoint_Line(pt0_B_, pt1_B_, line_plane_intersection);
				Vector3f v1 = line_plane_intersection - pt1;
				float dist_sq = MyMath::dot(v1, v1);
				float best_dist = dist_sq;
				reference_pt = pt1;

				Vector3f pt2 = Utils::getClosestPoint_Line(pt1_B_, pt2_B_, line_plane_intersection);
				Vector3f v2 = line_plane_intersection - pt2;
				dist_sq = MyMath::dot(v2, v2);
				if (dist_sq < best_dist)
				{
					reference_pt = pt2;
					best_dist = dist_sq;
				}

				Vector3f pt3 = Utils::getClosestPoint_Line(pt2_B_, pt0_B_, line_plane_intersection);
				Vector3f v3 = line_plane_intersection - pt3;
				dist_sq = MyMath::dot(v3, v3);
				if (dist_sq < best_dist)
				{
					reference_pt = pt3;
					best_dist = dist_sq;
				}
			}
			return checkTriangleWithSphere(pt0_B_, pt1_B_, pt2_B_, best_center, radius_A_);
		}

		{	// My method (brute force)	// no intersection: ~0.0150ms, have intersection: ~0.0085ms
			Vector3f best_center = Utils::getClosestPoint_Line(base_center_A, tip_center_A, pt0_B_);
			CollisionResult ret;
			ret = checkTriangleWithSphere(pt0_B_, pt1_B_, pt2_B_, best_center, radius_A_);
			if (ret.isCollided)
				return ret;

			best_center = Utils::getClosestPoint_Line(base_center_A, tip_center_A, pt1_B_);
			ret = checkTriangleWithSphere(pt0_B_, pt1_B_, pt2_B_, best_center, radius_A_);
			if (ret.isCollided)
				return ret;

			best_center = Utils::getClosestPoint_Line(base_center_A, tip_center_A, pt2_B_);
			ret = checkTriangleWithSphere(pt0_B_, pt1_B_, pt2_B_, best_center, radius_A_);
			if (ret.isCollided)
				return ret;

			return ret;
		}
	}
	// ========================Implementation===========================
#pragma endregion

}
