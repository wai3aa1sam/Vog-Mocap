#include "vogpch.h"
#include "OBBShape.h"

#include "Vog/Physics/Collision/Collision.h"
#include "Vog/Physics/Collision/CollisionResult.h"

#include "Vog/Physics/Utils/Geometry3D.h"

namespace vog {
	OBBShape::OBBShape(const Vector3f& center_, const Vector3f& size_)
		:
		Shape(ShapeType::OBB), m_center(center_), m_size(size_), m_rotationMatrix(Matrix3f(1.0f))
	{
#ifdef VOG_ENABLE_SHAPE_NAME
		this->m_debug_name = "OBB";
#endif // Enable_Shape_Name
	}
	OBBShape::OBBShape(const Vector3f& center_, const Vector3f& size_, const Vector3f& rotation_)
		:
		Shape(ShapeType::OBB),  m_center(center_), m_size(size_), m_rotationMatrix(MyMath::eulerToMatrix3f(rotation_))
	{
#ifdef VOG_ENABLE_SHAPE_NAME
		this->m_debug_name = "OBB";
#endif // Enable_Shape_Name
	}

	OBBShape::~OBBShape()
	{
	}

	void OBBShape::updateProperties(const BodyTransform& transform_)
	{
		m_rotationMatrix = MyMath::toMatrix3f(transform_.orientation);
	}

	Vector3f OBBShape::getClosestPoint(const Vector3f& point_, const Vector3f& position_)
	{
		return Utils::getClosestPoint_OBB(m_center + position_, m_size, m_rotationMatrix, point_);
	}

	CollisionResult OBBShape::collide(const BodyTransform* pTransform_A_, const Shape* pShape_B_, const BodyTransform* pTransform_B_) const
	{
		return pShape_B_->collide(pTransform_B_, this, pTransform_A_);
	}

	CollisionResult OBBShape::collide(const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkAABBWithOBB(pAABB_B_, pTransform_B_, this, pTransform_A_);
	}

	CollisionResult OBBShape::collide(const BodyTransform* pTransform_A_, const CapsuleShape* pCapsule_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkCapsuleWithOBB(pCapsule_B_, pTransform_B_, this, pTransform_A_);
	}

	CollisionResult OBBShape::collide(const BodyTransform* pTransform_A_, const MeshShape* pMesh_B_, const BodyTransform* pTransform_B_) const
	{
		return CollisionResult();
	}

	CollisionResult OBBShape::collide(const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkOBBWithOBB(this, pTransform_A_, pOBB_B_, pTransform_B_);
	}

	CollisionResult OBBShape::collide(const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkOBBWithPlane(this, pTransform_A_, pPlane_B_, pTransform_B_);
	}

	CollisionResult OBBShape::collide(const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkSphereWithOBB(pSphere_B_, pTransform_B_, this, pTransform_A_);
	}
	
	CollisionResult OBBShape::collide(const BodyTransform* pTransform_A_, const TriangleShape* pTriangle_B_, const BodyTransform* pTransform_B_) const
	{
		return Collision::checkTriangleWithOBB(pTriangle_B_, pTransform_B_, this, pTransform_A_);
	}
}