#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Shape.h"

namespace vog {

	class VOG_API AABBShape : public Shape
	{
		friend class Collision;
		friend struct SATInterval;
		friend class Raycast;
	public:
		AABBShape(const Vector3f& center_, const Vector3f& size_);
		AABBShape(const Vector3f& min_, const Vector3f& max_, const Vector3f& center_);

		virtual ~AABBShape();

		//void destroy();
		//void init();

		Vector3f getMin(const Vector3f& position_ = {0.0f, 0.0f, 0.0f}) const;
		Vector3f getMax(const Vector3f& position_ = {0.0f, 0.0f, 0.0f}) const;
		Vector3f getClosestPoint(const Vector3f& point_, const Vector3f& position_ = { 0.0f, 0.0f, 0.0f });

		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const Shape* pShape_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const CapsuleShape* pCapsule_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const MeshShape* pMesh_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_) const override;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const TriangleShape* pTriangle_B_, const BodyTransform* pTransform_B_) const override;

		void set(const Vector3f& min_, const Vector3f& max_, const Vector3f& center_ = { 0.0f, 0.0f, 0.0f });

		virtual ShapeType getType() const override { return ShapeType::AABB; }

#ifdef VOG_ENABLE_SHAPE_NAME
		virtual std::string getName() const override { return "AABB"; }
#endif // Enable_Shape_Name

	private:
		Vector3f m_center = { 0.0f, 0.0f, 0.0f };
		Vector3f m_size = { 0.5f, 0.5f, 0.5f };
	};
}