#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Shape.h"

namespace vog {

	class VOG_API OBBShape : public Shape
	{
		friend class Collision;
		friend struct SATInterval;
		friend class Raycast;
	public:
		OBBShape(const Vector3f& center_, const Vector3f& size_);
		OBBShape(const Vector3f& center_, const Vector3f& size_, const Vector3f& rotation_);

		virtual ~OBBShape();

		//void destroy();
		//void init();

		virtual void updateProperties(const BodyTransform& transform_) override;

		Vector3f getClosestPoint(const Vector3f& point_, const Vector3f& position_ = { 0.0f, 0.0f, 0.0f });

		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const Shape* pShape_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const CapsuleShape* pCapsule_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const MeshShape* pMesh_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const TriangleShape* pTriangle_B_, const BodyTransform* pTransform_B_) const override;

		virtual ShapeType getType() const override { return ShapeType::OBB; }

#ifdef VOG_ENABLE_SHAPE_NAME
		virtual std::string getName() const override { return "OBB"; }
#endif // Enable_Shape_Name

	private:
		Vector3f m_center = { 0.0f, 0.0f, 0.0f };
		Vector3f m_size = { 0.5f, 0.5f, 0.5f };

		Matrix3f m_rotationMatrix = Matrix3f(1.0f);
	};
}