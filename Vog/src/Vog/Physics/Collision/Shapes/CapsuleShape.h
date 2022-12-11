#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Shape.h"

namespace vog {

	class VOG_API CapsuleShape : public Shape
	{
		friend class Collision;
		friend class Raycast;

	public:
		//CapsuleShape(const Vector3f& base_, const Vector3f& tip_, float height_, float radius_);
		CapsuleShape(const Vector3f& center_, const Vector3f& normal_, float height_, float radius_);

		virtual ~CapsuleShape();

		//void destroy();
		//void init();
		virtual void updateProperties(const BodyTransform& transform_) override;

		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const Shape* pShape_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const CapsuleShape* pCapsule_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const MeshShape* pMesh_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const TriangleShape* pTriangle_B_, const BodyTransform* pTransform_B_) const override;

		virtual ShapeType getType() const override { return ShapeType::Capsule; }

#ifdef VOG_ENABLE_SHAPE_NAME
		virtual std::string getName() const override { return "Capsule"; }
#endif // Enable_Shape_Name

	private:
		//Vector3f m_base = { 0.0f, 0.0f, 0.0f };
		//Vector3f m_tip = { 0.0f, 1.0f, 0.0f };
		
		Vector3f m_center = { 0.0f, 0.0f, 0.0f };
		Vector3f m_normal= { 0.0f, 1.0f, 0.0f };

		float m_height = 2.0f;
		float m_radius = 0.5f;
	};
}