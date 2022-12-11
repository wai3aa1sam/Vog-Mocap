#include "vogpch.h"
#include "CapsuleShape.h"

#include "Vog/Physics/Collision/Collision.h"
#include "Vog/Physics/Collision/CollisionResult.h"

#include "Vog/Physics/Utils/Geometry3D.h"

namespace vog {

//	CapsuleShape::CapsuleShape(const Vector3f& base_, const Vector3f& tip_, float height_, float radius_)
//		:
//		Shape(ShapeType::Capsule), m_base(base_), m_tip(tip_), m_height(height_), m_radius(radius_)
//	{
//#ifdef VOG_ENABLE_SHAPE_NAME
//		this->m_debug_name = "Capsule";
//#endif // VOG_ENABLE_SHAPE_NAME
//	}

	CapsuleShape::CapsuleShape(const Vector3f& center_, const Vector3f& normal_, float height_, float radius_)
		:
		Shape(ShapeType::Capsule), m_center(center_), m_normal(normal_), m_height(height_), m_radius(radius_)
	{
#ifdef VOG_ENABLE_SHAPE_NAME
		this->m_debug_name = "Capsule";
#endif // VOG_ENABLE_SHAPE_NAME
	}

	CapsuleShape::~CapsuleShape()
	{
	}

	void CapsuleShape::updateProperties(const BodyTransform& transform_)
	{
		m_normal = transform_.orientation * Vector3f(0.0f, 1.0f, 0.0f);
	}

	CollisionResult CapsuleShape::collide(const BodyTransform* pTransform_A_, const Shape* pShape_B_, const BodyTransform* pTransform_B_) const
	{
		return pShape_B_->collide(pTransform_B_, this, pTransform_A_);
	}

	CollisionResult CapsuleShape::collide(const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkCapsuleWithAABB(this, pTransform_A_, pAABB_B_, pTransform_B_);
	}

	CollisionResult CapsuleShape::collide(const BodyTransform* pTransform_A_, const CapsuleShape* pCapsule_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkCapsuleWithCapsule(this, pTransform_A_, pCapsule_B_, pTransform_B_);
	}

	CollisionResult CapsuleShape::collide(const BodyTransform* pTransform_A_, const MeshShape* pMesh_B_, const BodyTransform* pTransform_B_) const
	{
		return {};
	}

	CollisionResult CapsuleShape::collide(const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkCapsuleWithOBB(this, pTransform_A_, pOBB_B_, pTransform_B_);
	}

	CollisionResult CapsuleShape::collide(const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkCapsuleWithPlane(this, pTransform_A_, pPlane_B_, pTransform_B_);
	}

	CollisionResult CapsuleShape::collide(const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkCapsuleWithSphere(this, pTransform_A_, pSphere_B_, pTransform_B_);
	}

	CollisionResult CapsuleShape::collide(const BodyTransform* pTransform_A_, const TriangleShape* pTriangle_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkCapsuleWithTriangle(this, pTransform_A_, pTriangle_B_, pTransform_B_);
	}
	
	
}