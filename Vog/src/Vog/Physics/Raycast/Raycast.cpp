#include "vogpch.h"
#include "Raycast.h"

#include "RaycastResult.h"

#include "Vog/Physics/Body/BodyTransform.h"

#include "Vog/Physics/Collision/Shapes/AABBShape.h"
#include "Vog/Physics/Collision/Shapes/SphereShape.h"
#include "Vog/Physics/Collision/Shapes/TriangleShape.h"
#include "Vog/Physics/Collision/Shapes/OBBShape.h"
#include "Vog/Physics/Collision/Shapes/PlaneShape.h"

#include "Vog/Physics/Utils/Geometry3D.h"

namespace vog {
	bool Raycast::isHitted(const Ray& ray_, const SphereShape* pSphere_, const BodyTransform* pTransform_, RaycastResult& result_)
	{
		auto center = pSphere_->m_center + pTransform_->position;
		auto radius = pSphere_->m_radius;
		return isHittedSphere(ray_, center, radius, result_);
	}

	bool Raycast::isHitted(const Ray& ray_, const AABBShape* pAABB_, const BodyTransform* pTransform_, RaycastResult& result_)
	{
		Vector3f min_pt = pAABB_->getMin(pTransform_->position);
		Vector3f max_pt = pAABB_->getMax(pTransform_->position);
		return isHittedAABB(ray_, min_pt, max_pt, result_);
	}

	bool Raycast::isHitted(const Ray& ray_, const OBBShape* pOBB_, const BodyTransform* pTransform_, RaycastResult& result_)
	{
		auto obb_center = pOBB_->m_center + pTransform_->position;
		auto obb_size = pOBB_->m_size;
		auto& rotationMatrix = pOBB_->m_rotationMatrix;
		return isHittedOBB(ray_, obb_center, obb_size, rotationMatrix, result_);
	}

	bool Raycast::isHitted(const Ray& ray_, const PlaneShape* pPlane_, const BodyTransform* pTransform_, RaycastResult& result_)
	{
		auto normal = MyMath::toMatrix3f(pTransform_->orientation) * Vector3f(0.0f, 1.0f, 0.0f);
		normal = MyMath::normalize(normal);
		float distance = MyMath::magnitude(pTransform_->position);
		if (pTransform_->position.y < 0.0f)
			distance *= -1.0f;

		return isHittedPlane(ray_, normal, distance, result_);
	}

	bool Raycast::isHitted(const Ray& ray_, const TriangleShape* pTriangle_, const BodyTransform* pTransform_, RaycastResult& result_)
	{
		Matrix4f transform = pTransform_->getTransform();
		auto pt0 = transform * Vector4f(pTriangle_->m_pt0, 1.0f);
		auto pt1 = transform * Vector4f(pTriangle_->m_pt1, 1.0f);
		auto pt2 = transform * Vector4f(pTriangle_->m_pt2, 1.0f);
		return isHittedTriangle(ray_, pt0, pt1, pt2, result_);
	}

	bool Raycast::isHittedSphere(const Ray& ray_, const Vector3f& center_, float radius_, RaycastResult& result_)
	{
		RaycastResult& ret = result_;

		auto center = center_;
		auto radius_sq = radius_ * radius_;

		auto dir_to_center = center - ray_.origin;
		auto dist_to_center_sq = MyMath::magnitude_square(dir_to_center);

		// distance of projected point (closest point) on ray
		auto dist_proj = MyMath::dot(dir_to_center, ray_.direction);	// direction of ray should be normalized

		auto dist_perp_sq = dist_to_center_sq - dist_proj * dist_proj;

		if (dist_perp_sq > radius_sq)									// no intersection
			return false;

		float radius_proj = MyMath::sqrt(radius_sq - dist_perp_sq);

		if (dist_to_center_sq < radius_sq)								// ray is inside, have collision
			ret.t = dist_proj + radius_proj;
		else
			ret.t = dist_proj - radius_proj;

		ret.point = ray_.origin + ret.t * ray_.direction;
		ret.normal = MyMath::normalize(ret.point - center);
		return true;
	}

	bool Raycast::isHittedAABB(const Ray& ray_, const Vector3f& min_pt_, const Vector3f& max_pt_, RaycastResult& result_)
	{
		RaycastResult& ret = result_;

		Vector3f min_pt = min_pt_;
		Vector3f max_pt = max_pt_;

		auto dir_ray = ray_.direction + MyMath::epsilon;		// avoid divide 0

		// p(t) = p0 + dir * t, 
		std::array<float, 6> ts;
		ts[0] = (min_pt.x - ray_.origin.x) / (dir_ray.x);		// t_min_x 
		ts[1] = (max_pt.x - ray_.origin.x) / (dir_ray.x);		// t_max_x 
		ts[2] = (min_pt.y - ray_.origin.y) / (dir_ray.y);		// t_min_y 
		ts[3] = (max_pt.y - ray_.origin.y) / (dir_ray.y);		// t_max_y 
		ts[4] = (min_pt.z - ray_.origin.z) / (dir_ray.z);		// t_min_z 
		ts[5] = (max_pt.z - ray_.origin.z) / (dir_ray.z);		// t_max_z 

		float t_min = MyMath::max(
			MyMath::max(MyMath::min(ts[0], ts[1]), MyMath::min(ts[2], ts[3])),
			MyMath::min(ts[4], ts[5])
		);

		float t_max = MyMath::min(
			MyMath::min(MyMath::max(ts[0], ts[1]), MyMath::max(ts[2], ts[3])),
			MyMath::max(ts[4], ts[5])
		);

		if (t_max < 0.0f || t_min > t_max)		// no intersection
		{
			return false;
		}

		if (t_min < 0.0f)						// inside aabb
			ret.t = t_max;
		else
			ret.t = t_min;

		std::array<Vector3f, 6> normals = {
			Vector3f{-1.0f, 0.0f, 0.0f}, Vector3f{1.0f, 0.0f, 0.0f},
			Vector3f{0.0f, -1.0f, 0.0f}, Vector3f{0.0f, 1.0f, 0.0f},
			Vector3f{0.0f, 0.0f, -1.0f}, Vector3f{0.0f, 0.0f, 1.0f},
		};

		for (int i = 0; i < normals.size(); i++)
		{
			if (MyMath::isEqualTo(ret.t, ts[i]))
				ret.normal = normals[i];
		}

		ret.point = ray_.origin + ret.t * ray_.direction;

		return true;
	}

	bool Raycast::isHittedOBB(const Ray& ray_, const Vector3f& center_, const Vector3f& size_, const Matrix3f& rotationMatrix_, RaycastResult& result_)
	{
		RaycastResult& ret = result_;

		auto dir_to_center = center_ - ray_.origin;
		auto obb_size = size_;
		auto& rotationMatrix = rotationMatrix_;

		std::array<Vector3f, 3> local_axes;
		local_axes[0] = { rotationMatrix[0][0], rotationMatrix[0][1], rotationMatrix[0][2] };
		local_axes[1] = { rotationMatrix[1][0], rotationMatrix[1][1], rotationMatrix[1][2] };
		local_axes[2] = { rotationMatrix[2][0], rotationMatrix[2][1], rotationMatrix[2][2] };

		Vector3f dir_proj = { MyMath::dot(ray_.direction, local_axes[0]) , MyMath::dot(ray_.direction, local_axes[1]), MyMath::dot(ray_.direction, local_axes[2]) };
		//dir_proj += MyMath::epsilon;
		Vector3f to_center_proj = { MyMath::dot(dir_to_center, local_axes[0]) , MyMath::dot(dir_to_center, local_axes[1]), MyMath::dot(dir_to_center, local_axes[2]) };

		std::array<float, 6> ts;

		for (int i = 0; i < local_axes.size(); i++)
		{
			if (MyMath::isEqualTo(dir_proj[i], 0.0f))												// parallel to hitted axis
			{
				if (-to_center_proj[i] < -obb_size[i] || -to_center_proj[i] > obb_size[i])			// check whether the ray is outside, the proj is -ve(diff dir with slab axis)
					return false;																	// early out

				dir_proj[i] = MyMath::epsilon;
			}

			ts[2 * i + 0] = (to_center_proj[i] + obb_size[i]) / dir_proj[i];						// min (depends on the ray on left / right side)
			ts[2 * i + 1] = (to_center_proj[i] - obb_size[i]) / dir_proj[i];						// max (depends on the ray on left / right side)
		}

		// same as aabb
		float t_min = MyMath::max(
			MyMath::max(MyMath::min(ts[0], ts[1]), MyMath::min(ts[2], ts[3])),
			MyMath::min(ts[4], ts[5])
		);

		float t_max = MyMath::min(
			MyMath::min(MyMath::max(ts[0], ts[1]), MyMath::max(ts[2], ts[3])),
			MyMath::max(ts[4], ts[5])
		);

		if (t_max < 0.0f || t_min > t_max)		// no intersection
		{
			return false;
		}

		if (t_min < 0.0f)						// inside aabb
			ret.t = t_max;
		else
			ret.t = t_min;

		std::array<Vector3f, 6> normals = {
			-local_axes[0], local_axes[0],
			-local_axes[1], local_axes[1],
			-local_axes[2], local_axes[2],
		};

		for (int i = 0; i < normals.size(); i++)
		{
			if (MyMath::isEqualTo(ret.t, ts[i]))
				ret.normal = normals[i];
		}

		ret.point = ray_.origin + ret.t * ray_.direction;

		return true;
	}

	bool Raycast::isHittedPlane(const Ray& ray_, const Vector3f& normal_, float distance_, RaycastResult& result_)
	{
		RaycastResult& ret = result_;

		auto normal = normal_;
		auto distance = distance_;

		float dir_proj = MyMath::dot(ray_.direction, normal);
		if (dir_proj >= 0.0f)													// same direction
			return false;

		float dist_proj = MyMath::dot(ray_.origin, normal);
		float t = (distance - dist_proj) / dir_proj;							// if the ray origin is above, the t will be -ve, means no interesction

		if (t < 0.0f)															// no interesction
			return false;

		ret.t = t;
		ret.point = ray_.origin + ret.t * ray_.direction;
		ret.normal = normal;

		return true;
	}

	bool Raycast::isHittedTriangle(const Ray& ray_, const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, RaycastResult& result_)
	{
		RaycastResult& ret = result_;
		// form a plane
		Vector3f normal = Utils::getNormal_Triangle(pt0_, pt1_, pt2_);
		float distance = MyMath::dot(pt0_, normal);
		
		if (!isHittedPlane(ray_, normal, distance, ret))
			return false;

		Vector3f barycentric = Utils::getBarycentric(pt0_, pt1_, pt2_, ret.point);
		if (barycentric.x <= 0.0f || barycentric.x >= 1.0f ||
			barycentric.y <= 0.0f || barycentric.y >= 1.0f ||
			barycentric.z <= 0.0f || barycentric.z >= 1.0f)
			return false;

		return true;
	}
}
