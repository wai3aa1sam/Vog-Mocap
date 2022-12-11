#include "vogpch.h"
#include "SphereShape.h"

#include "Vog/Physics/Collision/Collision.h"
#include "Vog/Physics/Collision/CollisionResult.h"

#include "Vog/Physics/Utils/Geometry3D.h"

namespace vog {

	SphereShape::SphereShape(const Vector3f& center_, float radius_)
		:
		Shape(ShapeType::Sphere), m_center(center_), m_radius(radius_)
	{
#ifdef VOG_ENABLE_SHAPE_NAME
		this->m_debug_name = "Sphere";
#endif // VOG_ENABLE_SHAPE_NAME
	}

	SphereShape::~SphereShape()
	{
	}

	Vector3f SphereShape::getClosestPoint(const Vector3f& point_, const Vector3f& position_)
	{
		auto center = m_center + position_;
		return Utils::getClosestPoint_Sphere(center, m_radius, point_);
	}

	CollisionResult SphereShape::collide(const BodyTransform* pTransform_A_, const Shape* pShape_B_, const BodyTransform* pTransform_B_) const
	{
		//return this->collide(pTransform_A_, pShape_B_, pTransform_B_);
		return pShape_B_->collide(pTransform_B_, this, pTransform_A_);
	}

	CollisionResult SphereShape::collide(const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkSphereWithAABB(this, pTransform_A_, pAABB_B_, pTransform_B_);
	}

	CollisionResult SphereShape::collide(const BodyTransform* pTransform_A_, const CapsuleShape* pCapsule_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkCapsuleWithSphere(pCapsule_B_, pTransform_B_, this, pTransform_A_);
	}

	CollisionResult SphereShape::collide(const BodyTransform* pTransform_A_, const MeshShape* pMesh_B_, const BodyTransform* pTransform_B_) const
	{
		return CollisionResult();
	}

	CollisionResult SphereShape::collide(const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkSphereWithOBB(this, pTransform_A_, pOBB_B_, pTransform_B_);
	}

	CollisionResult SphereShape::collide(const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkSphereWithPlane(this, pTransform_A_, pPlane_B_, pTransform_B_);
	}

	CollisionResult SphereShape::collide(const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkSphereWithSphere(this, pTransform_A_, pSphere_B_, pTransform_B_);
	}
	
	CollisionResult SphereShape::collide(const BodyTransform* pTransform_A_, const TriangleShape* pTriangle_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkTriangleWithSphere(pTriangle_B_, pTransform_B_, this, pTransform_A_);
	}
}