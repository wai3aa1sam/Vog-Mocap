#include "vogpch.h"
#include "PlaneShape.h"

#include "Vog/Physics/Collision/Collision.h"
#include "Vog/Physics/Collision/CollisionResult.h"

namespace vog {

	PlaneShape::PlaneShape(const Vector3f& normal_, float distance_)
		:
		Shape(ShapeType::Plane), m_normal(normal_), m_distance(distance_)
	{
#ifdef VOG_ENABLE_SHAPE_NAME
		this->m_debug_name = "Plane";
#endif // Enable_Shape_Name
	}
	PlaneShape::~PlaneShape()
	{
	}

	void PlaneShape::updateProperties(const BodyTransform& transform_)
	{
		m_normal = transform_.orientation * Vector3f(0.0f, 1.0f, 0.0f);
	}

	Vector3f PlaneShape::getClosestPoint(const Vector3f& point_)
	{
		Vector3f normal = m_normal;
		float distance = m_distance;

		float distanceToPoint = MyMath::dot(normal, point_);
		return point_ - normal * (distanceToPoint - distance);
	}

	CollisionResult PlaneShape::collide(const BodyTransform* pTransform_A_, const Shape* pShape_B_, const BodyTransform* pTransform_B_) const
	{
		//return this->collide(pTransform_A_, pShape_B_, pTransform_B_);
		return pShape_B_->collide(pTransform_B_, this, pTransform_A_);
	}

	CollisionResult PlaneShape::collide(const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkAABBWithPlane(pAABB_B_, pTransform_B_, this, pTransform_A_);
	}

	CollisionResult PlaneShape::collide(const BodyTransform* pTransform_A_, const CapsuleShape* pCapsule_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkCapsuleWithPlane(pCapsule_B_, pTransform_B_, this, pTransform_A_);
	}

	CollisionResult PlaneShape::collide(const BodyTransform* pTransform_A_, const MeshShape* pMesh_B_, const BodyTransform* pTransform_B_) const
	{
		return CollisionResult();
	}

	CollisionResult PlaneShape::collide(const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkOBBWithPlane(pOBB_B_, pTransform_B_, this, pTransform_A_);
	}
	
	CollisionResult PlaneShape::collide(const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkPlaneWithPlane(this, pTransform_A_, pPlane_B_, pTransform_B_);
	}

	CollisionResult PlaneShape::collide(const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkSphereWithPlane(pSphere_B_, pTransform_B_, this, pTransform_A_);
	}

	CollisionResult PlaneShape::collide(const BodyTransform* pTransform_A_, const TriangleShape* pTriangle_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkTriangleWithPlane(pTriangle_B_, pTransform_B_, this, pTransform_A_);
	}
}