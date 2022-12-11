#include "vogpch.h"
#include "AABBShape.h"

#include "Vog/Physics/Collision/Collision.h"
#include "Vog/Physics/Collision/CollisionResult.h"

#include "Vog/Physics/Utils/Geometry3D.h"

namespace vog {
	AABBShape::AABBShape(const Vector3f& center_, const Vector3f& size_)
		:
		Shape(ShapeType::AABB), m_center(center_), m_size(size_)
	{
#ifdef VOG_ENABLE_SHAPE_NAME
		this->m_debug_name = "AABB";
#endif // VOG_ENABLE_SHAPE_NAME

	}

	AABBShape::AABBShape(const Vector3f& min_, const Vector3f& max_, const Vector3f& center_)
		:
		Shape(ShapeType::AABB), m_center(center_), m_size((max_ - min_) / 2.0f)
	{
#ifdef VOG_ENABLE_SHAPE_NAME
		this->m_debug_name = "AABB";
#endif // VOG_ENABLE_SHAPE_NAME
	}

	AABBShape::~AABBShape()
	{
	}

	Vector3f AABBShape::getMin(const Vector3f& position_) const
	{
		return Utils::getMinPoint_AABB(m_center + position_, m_size);
	}
	Vector3f AABBShape::getMax(const Vector3f& position_) const
	{
		return Utils::getMaxPoint_AABB(m_center + position_, m_size);
	}

	Vector3f AABBShape::getClosestPoint(const Vector3f& point_, const Vector3f& position_) 
	{
		return Utils::getClosestPoint_AABB(m_center + position_, m_size, point_);
	}

	CollisionResult AABBShape::collide(const BodyTransform* pTransform_A_, const Shape* pShape_B_, const BodyTransform* pTransform_B_) const
	{
		return pShape_B_->collide(pTransform_B_, this, pTransform_A_);
	}

	CollisionResult AABBShape::collide(const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkAABBWithAABB(this, pTransform_A_, pAABB_B_, pTransform_B_);
	}

	CollisionResult AABBShape::collide(const BodyTransform* pTransform_A_, const CapsuleShape* pCapsule_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkCapsuleWithAABB(pCapsule_B_, pTransform_B_, this, pTransform_A_);
	}
	
	CollisionResult AABBShape::collide(const BodyTransform* pTransform_A_, const MeshShape* pMesh_B_, const BodyTransform* pTransform_B_) const
	{
		return CollisionResult();
	}

	CollisionResult AABBShape::collide(const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkAABBWithOBB(this, pTransform_A_, pOBB_B_, pTransform_B_);
	}

	CollisionResult AABBShape::collide(const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkAABBWithPlane(this, pTransform_A_, pPlane_B_, pTransform_B_);
	}

	CollisionResult AABBShape::collide(const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkSphereWithAABB(pSphere_B_, pTransform_B_, this, pTransform_A_);
	}

	CollisionResult AABBShape::collide(const BodyTransform* pTransform_A_, const TriangleShape* pTriangle_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkTriangleWithAABB(pTriangle_B_, pTransform_B_, this, pTransform_A_);
	}

	void AABBShape::set(const Vector3f& min_, const Vector3f& max_, const Vector3f& center_)
	{
		m_center = center_;
		m_size = (max_ - min_) * 0.5f;
	}
}