#include "vogpch.h"
#include "SATInterval.h"

namespace vog {
	SATInterval SATInterval::getInterval(const AABBShape* pAABB_, const BodyTransform* pTransform_, const Vector3f& axis_)
	{
		auto min = pAABB_->getMin(pTransform_->position);
		auto max = pAABB_->getMax(pTransform_->position);

		//		0--------6
		//		|\		 |\
		//		| \		 | \
		//		|  1--------3
		//      |  |     |  |
		//	  min--|-----5  |
		//		 \ |      \ |
		//		  \2-------\4

		// all vertices of aabb
		std::array<Vector3f, 7> vertices = {
			//Vector3f{min.x, min.y, min.z},
			Vector3f{min.x, max.y, min.z},		// 0
			Vector3f{min.x, max.y, max.z},		// 1
			Vector3f{min.x, min.y, max.z},		// 2
			Vector3f{max.x, max.y, max.z},		// 3
			Vector3f{max.x, min.y, max.z},		// 4
			Vector3f{max.x, min.y, min.z},		// 5
			Vector3f{max.x, max.y, min.z},		// 6
		};

		SATInterval ret;
		auto proj = MyMath::dot(min, axis_);
		ret.min = proj;
		ret.max = proj;

		for (size_t i = 0; i < vertices.size(); i++)
		{
			proj = MyMath::dot(vertices[i], axis_);
			if (proj < ret.min)
				ret.min = proj;
			if (proj > ret.max)
				ret.max = proj;
		}

		return ret;
	}
	SATInterval SATInterval::getInterval(const OBBShape* pOBB_, const BodyTransform* pTransform_, const Vector3f& axis_)
	{
		auto center = pOBB_->m_center + pTransform_->position;
		const Vector3f& size = pOBB_->m_size;

		std::array<Vector3f, 8> vertices;

		//axis
		std::array<Vector3f, 3> axis;
		axis[0] = Vector3f{ pOBB_->m_rotationMatrix[0][0], pOBB_->m_rotationMatrix[0][1], pOBB_->m_rotationMatrix[0][2]};
		axis[1] = Vector3f{ pOBB_->m_rotationMatrix[1][0], pOBB_->m_rotationMatrix[1][1], pOBB_->m_rotationMatrix[1][2]};
		axis[2] = Vector3f{ pOBB_->m_rotationMatrix[2][0], pOBB_->m_rotationMatrix[2][1], pOBB_->m_rotationMatrix[2][2]};

		//		7--------6
		//		|\		 |\
		//		| \		 | \
		//		|  1--------0
		//      |  |     |  |
		//		4--|-----5  |
		//		 \ |      \ |
		//		  \2-------\3

		vertices[0] = center + axis[0] * size.x + axis[1] * size.y + axis[2] * size.z;
		vertices[1] = center - axis[0] * size.x + axis[1] * size.y + axis[2] * size.z;
		vertices[2] = center - axis[0] * size.x - axis[1] * size.y + axis[2] * size.z;
		vertices[3] = center + axis[0] * size.x - axis[1] * size.y + axis[2] * size.z;
		vertices[4] = center - axis[0] * size.x - axis[1] * size.y - axis[2] * size.z;
		vertices[5] = center + axis[0] * size.x - axis[1] * size.y - axis[2] * size.z;
		vertices[6] = center + axis[0] * size.x + axis[1] * size.y - axis[2] * size.z;
		vertices[7] = center - axis[0] * size.x + axis[1] * size.y - axis[2] * size.z;

		SATInterval ret;
		auto proj = MyMath::dot(vertices[0], axis_);
		ret.min = proj;
		ret.max = proj;

		for (size_t i = 1; i < vertices.size(); i++)
		{
			proj = MyMath::dot(vertices[i], axis_);
			if (proj < ret.min)
				ret.min = proj;
			if (proj > ret.max)
				ret.max = proj;
		}

		return ret;
	}

	SATInterval SATInterval::getInterval(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const Vector3f& axis_)
	{
		SATInterval interval;
		float proj0 = MyMath::dot(pt0_, axis_);
		float proj1 = MyMath::dot(pt1_, axis_);
		float proj2 = MyMath::dot(pt2_, axis_);
		interval.min = proj0;
		interval.max = proj0;

		interval.min = MyMath::min(interval.min, proj1);
		interval.max = MyMath::max(interval.max, proj1);

		interval.min = MyMath::min(interval.min, proj2);
		interval.max = MyMath::max(interval.max, proj2);

		return interval;
	}

	bool SATInterval::isOverlapOnAxis(const AABBShape* pAABB_A_, const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_, const Vector3f& axis_)
	{
		SATInterval interval_aabb = getInterval(pAABB_A_, pTransform_A_, axis_);
		SATInterval interval_obb = getInterval(pOBB_B_, pTransform_B_, axis_);

		return (interval_aabb.max >= interval_obb.min) && (interval_obb.max >= interval_aabb.min);
	}

	bool SATInterval::isOverlapOnAxis(const OBBShape* pOBB_A_, const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_, const Vector3f& axis_)
	{
		SATInterval interval_obb_a = getInterval(pOBB_A_, pTransform_A_, axis_);
		SATInterval interval_obb_b = getInterval(pOBB_B_, pTransform_B_, axis_);

		return (interval_obb_a.max >= interval_obb_b.min) && (interval_obb_b.max >= interval_obb_a.min);
	}

	bool SATInterval::isOverlapOnAxis(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_, const Vector3f& axis_)
	{
		SATInterval interval_triangle = getInterval(pt0_, pt1_, pt2_, axis_);
		SATInterval interval_aabb = getInterval(pAABB_B_, pTransform_B_, axis_);

		return (interval_triangle.max >= interval_aabb.min) && (interval_aabb.max >= interval_triangle.min);
	}

	bool SATInterval::isOverlapOnAxis(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_, const Vector3f& axis_)
	{
		SATInterval interval_triangle = getInterval(pt0_, pt1_, pt2_, axis_);
		SATInterval interval_obb = getInterval(pOBB_B_, pTransform_B_, axis_);

		return (interval_triangle.max >= interval_obb.min) && (interval_obb.max >= interval_triangle.min);
	}
	bool SATInterval::isOverlapOnAxis(const Vector3f& pt0_A_, const Vector3f& pt1_A_, const Vector3f& pt2_A_, const Vector3f& pt0_B_, const Vector3f& pt1_B_, const Vector3f& pt2_B_, const Vector3f& axis_)
	{
		SATInterval interval_triangle_A = getInterval(pt0_A_, pt1_A_, pt2_A_, axis_);
		SATInterval interval_triangle_B = getInterval(pt0_B_, pt1_B_, pt2_B_, axis_);

		return (interval_triangle_A.max >= interval_triangle_B.min) && (interval_triangle_B.max >= interval_triangle_A.min);
	}
}