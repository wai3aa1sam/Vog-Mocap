#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Shape.h"

namespace vog {

	class VOG_API PlaneShape : public Shape
	{
		friend class Collision;
		friend class Raycast;
	public:
		PlaneShape(const Vector3f& normal_, float distance_);
		virtual ~PlaneShape();

		//void destroy();
		//void init();

		virtual void updateProperties(const BodyTransform& transform_) override;

		Vector3f getClosestPoint(const Vector3f& point_);

		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const Shape* pShape_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const CapsuleShape* pCapsule_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const MeshShape* pMesh_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const TriangleShape* pTriangle_B_, const BodyTransform* pTransform_B_) const override;

		virtual ShapeType getType() const override { return ShapeType::Plane; }

#ifdef VOG_ENABLE_SHAPE_NAME
		virtual std::string getName() const override { return "Plane"; }
#endif // Enable_Shape_Name

	private:
		Vector3f m_normal = { 0.0f, 1.0f, 0.0f };
		float m_distance = 0.0f;						// distance from world (0.0f, 0.0f, 0.0f)
	};
}