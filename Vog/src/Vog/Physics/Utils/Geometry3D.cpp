#include "vogpch.h"
#include "Geometry3D.h"

#include "Vog/Physics/Collision/CollisionResult.h"

namespace vog {

	Vector3f Utils::getClosestPoint_Line(const Vector3f& ln_start_, const Vector3f& ln_end_, const Vector3f& point_)
	{
		// from stack overflow: https://stackoverflow.com/questions/3120357/get-closest-point-to-a-line
		Vector3f ap = point_ - ln_start_;
		Vector3f ab = ln_end_ - ln_start_;

		float mag_sq_ab = MyMath::magnitude_square(ab);
		float ab_dot_ap = MyMath::dot(ap, ab);
		float distance = ab_dot_ap / mag_sq_ab;

		if (distance < 0.0f)
			return ln_start_;
		else if (distance > 1.0f)
			return ln_end_;
		else
			return
			ln_start_ + ab * distance;

		/*Vector3f dir_line = ln_end_ - ln_start_;
		dir_line = MyMath::normalize(dir_line);
		Vector3f to_pt = point_ - ln_start_;
		to_pt = MyMath::normalize(to_pt);

		float t = MyMath::dot(to_pt, dir_line);
		t = MyMath::min(t, 1.0f);
		t = MyMath::max(t, 0.0f);

		return ln_start_ + t * dir_line;*/
	}

	Vector3f Utils::getClosestPoint_Triangle(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const Vector3f& pt_)
	{
		if (isPointInTraingle(pt0_, pt1_, pt2_, pt_))
			return pt_;

		Vector3f closest_pt_0 = getClosestPoint_Line(pt0_, pt1_, pt_);
		Vector3f closest_pt_1 = getClosestPoint_Line(pt1_, pt2_, pt_);
		Vector3f closest_pt_2 = getClosestPoint_Line(pt2_, pt0_, pt_);

		// compare which closest point is the nearest to the point
		float dist_0_sq = MyMath::magnitude_square(pt_ - closest_pt_0);
		float dist_1_sq = MyMath::magnitude_square(pt_ - closest_pt_1);
		float dist_2_sq = MyMath::magnitude_square(pt_ - closest_pt_2);

		if (dist_0_sq < dist_1_sq && dist_0_sq < dist_2_sq)
			return closest_pt_0;
		else if (dist_1_sq < dist_0_sq && dist_1_sq < dist_2_sq)
			return closest_pt_1;

		return closest_pt_2;
	}

	Vector3f Utils::getClosestPoint_AABB_By_Center(const Vector3f& center_, const Vector3f& size_, const Vector3f& point_)
	{
		Vector3f ret = point_;

		Vector3f min = getMinPoint_AABB(center_, size_);
		Vector3f max = getMaxPoint_AABB(center_, size_);

		ret = MyMath::getMax(ret, min);
		ret = MyMath::getMin(ret, max);

		return ret;
	}

	Vector3f Utils::getClosestPoint_AABB(const Vector3f& min_pt_, const Vector3f& max_pt_, const Vector3f& point_)
	{
		Vector3f ret = point_;

		ret = MyMath::getMax(ret, min_pt_);
		ret = MyMath::getMin(ret, max_pt_);

		return ret;
	}

	Vector3f Utils::getClosestPoint_Plane(const Vector3f& normal_, float distance_, const Vector3f& point_)
	{
		Vector3f normal = normal_;
		float distance = distance_;

		float distanceToPoint = MyMath::dot(normal, point_);
		return point_ - normal * (distanceToPoint - distance);
	}

	Vector3f Utils::getClosestPoint_Sphere(const Vector3f& center_, float radius_, const Vector3f& point_)
	{
		auto center = center_;
		auto direction = point_ - center;
		auto distance_sq = MyMath::magnitude_square(direction);
		if (MyMath::isEqualTo(distance_sq, 0.0f))
			return center_;
		else if (distance_sq <= radius_ * radius_)
			return point_;
		else
			return center + direction / MyMath::sqrt(distance_sq) * radius_;
	}

	Vector3f Utils::getClosestPoint_OBB(const Vector3f& center_, const Vector3f& size_, const Matrix3f& rotationMatrix_, const Vector3f& point_)
	{
		Vector3f ret = center_;

		auto direction = point_ - (ret);
		for (int i = 0; i < 3; i++)		// cal the distance to the local axis (x, y, z) of obb
		{
			const float* orientation = &rotationMatrix_[i][0];
			Vector3f axis = { orientation[0], orientation[1], orientation[2] };
			float distance = MyMath::dot(direction, axis);

			if (distance > size_[i])
				distance = size_[i];
			if (distance < -size_[i])
				distance = -size_[i];

			ret = ret + axis * distance;
		}

		return ret;
	}

	bool Utils::isPointInTraingle(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const Vector3f& pt_)
	{
		auto to_pt0 = pt0_ - pt_;
		auto to_pt1 = pt1_ - pt_;
		auto to_pt2 = pt2_ - pt_;

		auto cross0 = MyMath::cross(to_pt0, to_pt1);
		auto cross1 = MyMath::cross(to_pt1, to_pt2);
		auto cross2 = MyMath::cross(to_pt2, to_pt0);

		if (MyMath::dot(cross0, cross1) < 0.0f || MyMath::dot(cross0, cross2) < 0.0f)		// not same direction, not in the tirangle
			return false;

		return true;
	}

	Vector3f Utils::getNormal_Triangle(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_)
	{
		return MyMath::normalize(MyMath::cross(pt2_ - pt0_, pt1_ - pt0_));
	}

	Vector3f Utils::getBarycentric(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const Vector3f& pt_)
	{
		auto to_pt_0 = pt_ - pt0_;
		auto to_pt_1 = pt_ - pt1_;
		auto to_pt_2 = pt_ - pt2_;

		Vector3f edge0 = pt1_ - pt0_;
		Vector3f edge1 = pt2_ - pt1_;
		Vector3f edge2 = pt0_ - pt2_;

		Vector3f v = perpendicular(-edge0, edge1);
		float a = 1.0f - MyMath::dot(-v, to_pt_0) / MyMath::dot(v, v);

		v = perpendicular(-edge1, edge2);
		float b = 1.0f - MyMath::dot(-v, to_pt_1) / MyMath::dot(v, v);

		v = perpendicular(-edge2, edge0);
		float c = 1.0f - MyMath::dot(-v, to_pt_2) / MyMath::dot(v, v);

		return Vector3f(a, b, c);
	}

	Vector3f Utils::project(const Vector3f& from_, const Vector3f& to_)
	{
		float dist_proj = MyMath::dot(from_, to_);
		float dist_to_sq = MyMath::magnitude_square(to_);

		return to_ * (dist_proj / dist_to_sq);
	}

	Vector3f Utils::perpendicular(const Vector3f& from_, const Vector3f& to_)
	{
		return from_ - project(from_, to_);
	}

	Vector3f Utils::getMinPoint_AABB(const Vector3f& center_, const Vector3f& size_)
	{
		Vector3f pt1 = center_ - size_;
		Vector3f pt2 = center_ + size_;

		return MyMath::getMin(pt1, pt2);
	}

	Vector3f Utils::getMaxPoint_AABB(const Vector3f& center_, const Vector3f& size_)
	{
		Vector3f pt1 = center_ - size_;
		Vector3f pt2 = center_ + size_;

		return MyMath::getMax(pt1, pt2);
	}

	Vector3f Utils::getCenter_AABB(const Vector3f& min_pt_, const Vector3f& max_pt_)
	{
		return (max_pt_ + min_pt_) * 0.5f;
	}
}


