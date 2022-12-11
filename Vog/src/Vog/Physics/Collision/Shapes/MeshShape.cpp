#include "vogpch.h"
#include "MeshShape.h"

#include "Vog/Physics/Collision/Collision.h"
#include "Vog/Physics/Collision/CollisionResult.h"

namespace vog {
	MeshShape::MeshShape()
		:
		Shape(ShapeType::Mesh)
	{
#ifdef VOG_ENABLE_SHAPE_NAME
		this->m_debug_name = "Mesh";
#endif // VOG_ENABLE_SHAPE_NAME
	}

	MeshShape::~MeshShape()
	{
	}
	
	CollisionResult MeshShape::collide(const BodyTransform* pTransform_A_, const Shape* pShape_B_, const BodyTransform* pTransform_B_) const
	{
		return pShape_B_->collide(pTransform_B_, this, pTransform_A_);
	}

	CollisionResult MeshShape::collide(const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_) const
	{
		return {};
	}

	CollisionResult MeshShape::collide(const BodyTransform* pTransform_A_, const CapsuleShape* pCapsule_B_, const BodyTransform* pTransform_B_) const
	{
		return {};
	}

	CollisionResult MeshShape::collide(const BodyTransform* pTransform_A_, const MeshShape* pMesh_B_, const BodyTransform* pTransform_B_) const
	{
		return {};
	}

	CollisionResult MeshShape::collide(const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_) const
	{
		return {};
	}

	CollisionResult MeshShape::collide(const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_) const
	{
		return {};
	}

	CollisionResult MeshShape::collide(const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_) const
	{
		return {};
	}

	CollisionResult MeshShape::collide(const BodyTransform* pTransform_A_, const TriangleShape* pTriangle_B_, const BodyTransform* pTransform_B_) const
	{
		return {};
	}
}