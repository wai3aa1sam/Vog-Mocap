#include "vogpch.h"
#include "TriangleShape.h"

#include "Vog/Physics/Collision/Collision.h"
#include "Vog/Physics/Collision/CollisionResult.h"

namespace vog {
	TriangleShape::TriangleShape(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_)
		:
		Shape(ShapeType::Triangle), m_pt0(pt0_), m_pt1(pt1_), m_pt2(pt2_)
	{
#ifdef VOG_ENABLE_SHAPE_NAME
		this->m_debug_name = "Triangle";
#endif // VOG_ENABLE_SHAPE_NAME
	}

	TriangleShape::~TriangleShape()
	{
	}
	Vector3f TriangleShape::getClosestPoint(const Vector3f& point_, const Vector3f& position_)
	{
		return Vector3f();
	}

	CollisionResult TriangleShape::collide(const BodyTransform* pTransform_A_, const Shape* pShape_B_, const BodyTransform* pTransform_B_) const
	{
		return pShape_B_->collide(pTransform_B_, this, pTransform_A_);
	}

	CollisionResult TriangleShape::collide(const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkTriangleWithAABB(this, pTransform_A_, pAABB_B_, pTransform_B_);
	}

	CollisionResult TriangleShape::collide(const BodyTransform* pTransform_A_, const CapsuleShape* pCapsule_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkCapsuleWithTriangle(pCapsule_B_, pTransform_B_, this, pTransform_A_);
	}

	CollisionResult TriangleShape::collide(const BodyTransform* pTransform_A_, const MeshShape* pMesh_B_, const BodyTransform* pTransform_B_) const
	{
		return CollisionResult();
	}

	CollisionResult TriangleShape::collide(const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkTriangleWithOBB(this, pTransform_A_, pOBB_B_, pTransform_B_);
	}

	CollisionResult TriangleShape::collide(const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkTriangleWithPlane(this, pTransform_A_, pPlane_B_, pTransform_B_);
	}

	CollisionResult TriangleShape::collide(const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkTriangleWithSphere(this, pTransform_A_, pSphere_B_, pTransform_B_);
	}

	CollisionResult TriangleShape::collide(const BodyTransform* pTransform_A_, const TriangleShape* pTriangle_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkTriangleWithTriangle(this, pTransform_A_, pTriangle_B_, pTransform_B_);
	}
}