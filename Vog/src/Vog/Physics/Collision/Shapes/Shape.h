#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Vog/Physics/Body/BodyTransform.h"

#include "Vog/Physics/Collision/CollisionCallbackBase.h"

#include <vector>

#define VOG_ENABLE_SHAPE_NAME

namespace vog {

	class AABBShape;
	class CapsuleShape;
	class SphereShape;
	class OBBShape;
	class PlaneShape;
	class TriangleShape;
	class MeshShape;

	struct CollisionResult;

	enum class ShapeType
	{
		Sphere,
		Plane,
		Box,
		Capsule,
		Triangle,
		Mesh,

		AABB,
		OBB,

		Count,
	};

	class VOG_API Shape
	{
		friend class CollisionManager;

	public:
		Shape(ShapeType type_);
		virtual ~Shape() = default;

		virtual void updateProperties(const BodyTransform& transform_) {};

		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const Shape* pShape_B_, const BodyTransform* pTransform_B_) const = 0;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_) const = 0;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const CapsuleShape* pCapsule_B_, const BodyTransform* pTransform_B_) const = 0;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const MeshShape* pMesh_B_, const BodyTransform* pTransform_B_) const = 0;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_) const = 0;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_) const = 0;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_) const = 0;
		virtual CollisionResult collide(const BodyTransform* pTransform_A_, const TriangleShape* pTriangle_B_, const BodyTransform* pTransform_B_) const = 0;


		virtual ShapeType getType() const = 0;

#ifdef VOG_ENABLE_SHAPE_NAME
		virtual std::string getName() const = 0;
#endif // Enable_Shape_Name

	protected:
		ShapeType m_type;

#ifdef VOG_ENABLE_SHAPE_NAME
		std::string m_debug_name;
#endif // Enable_Shape_Name

		// TODO: temporary collision callback 
		// should have id / collision id to perform the trigger stay and exit and pass his pair to the function!
		CollisionCallbackBase* m_pCollisionCallbackBase = nullptr;
		/*bool m_isColliding = false;
		bool m_isCollisionExited = false;*/
	};
}