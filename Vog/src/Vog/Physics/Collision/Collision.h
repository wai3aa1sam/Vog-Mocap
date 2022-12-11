#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

// References: 
// Game Physics Cookbook (2017)
// for capsule collision: https://wickedengine.net/2020/04/26/capsule-collision-detection/

namespace vog {

	class AABBShape;
	class CapsuleShape;
	class SphereShape;
	class OBBShape;
	class PlaneShape;
	class TriangleShape;

	struct CollisionResult;
	struct CollisionConfig;

	struct BodyTransform;

	class VOG_API Collision : public NonCopyable
	{
	public:
		static CollisionResult checkSphereWithSphere(const SphereShape* pSphere_A_, const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_);
		static CollisionResult checkSphereWithPlane(const SphereShape* pSphere_A_, const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_);
		static CollisionResult checkSphereWithAABB(const SphereShape* pSphere_A_, const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_);
		static CollisionResult checkSphereWithOBB(const SphereShape* pSphere_A_, const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_);
		static CollisionResult checkShpereWithPoint(const SphereShape* pSphere_A_, const BodyTransform* pTransform_A_, const Vector3f& pt_B_);

		static CollisionResult checkAABBWithAABB(const AABBShape* pAABB_A_,	const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_,const BodyTransform* pTransform_B_);
		static CollisionResult checkAABBWithOBB(const AABBShape* pAABB_A_, const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_);
		static CollisionResult checkAABBWithPoint(const AABBShape* pAABB_A_, const BodyTransform* pTransform_A_, const Vector3f& pt_B_);
		static CollisionResult checkAABBWithPlane(const AABBShape* pAABB_A_, const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_);

		static CollisionResult checkOBBWithOBB(const OBBShape* pOBB_A_, const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_);
		static CollisionResult checkOBBWithPlane(const OBBShape* pOBB_A_, const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_);

		static CollisionResult checkPlaneWithPlane(const PlaneShape* pPlane_A_, const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_);

		static CollisionResult checkTriangleWithSphere(const TriangleShape* pTriangle_A_, const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_);
		static CollisionResult checkTriangleWithPlane(const TriangleShape* pTriangle_A_, const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_);
		static CollisionResult checkTriangleWithAABB(const TriangleShape* pTriangle_A_, const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_);
		static CollisionResult checkTriangleWithOBB(const TriangleShape* pTriangle_A_, const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_);
		static CollisionResult checkTriangleWithTriangle(const TriangleShape* pTriangle_A_, const BodyTransform* pTransform_A_, const TriangleShape* pTriangle_B_, const BodyTransform* pTransform_B_);

		static CollisionResult checkCapsuleWithAABB(const CapsuleShape* pCapsule_A_, const BodyTransform* pTransform_A_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_);
		static CollisionResult checkCapsuleWithCapsule(const CapsuleShape* pCapsule_A_, const BodyTransform* pTransform_A_, const CapsuleShape* pCapsule_B_, const BodyTransform* pTransform_B_);
		static CollisionResult checkCapsuleWithOBB(const CapsuleShape* pCapsule_A_, const BodyTransform* pTransform_A_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_);
		static CollisionResult checkCapsuleWithPlane(const CapsuleShape* pCapsule_A_, const BodyTransform* pTransform_A_, const PlaneShape* pPlane_B_, const BodyTransform* pTransform_B_);
		static CollisionResult checkCapsuleWithSphere(const CapsuleShape* pCapsule_A_, const BodyTransform* pTransform_A_, const SphereShape* pSphere_B_, const BodyTransform* pTransform_B_);
		static CollisionResult checkCapsuleWithTriangle(const CapsuleShape* pCapsule_A_, const BodyTransform* pTransform_A_, const TriangleShape* pTriangle_B_, const BodyTransform* pTransform_B_);

		// implementation
		static CollisionResult checkSphereWithSphere(const Vector3f& center_A_, float radius_A_, const Vector3f& center_B_, float radius_B_);
		static CollisionResult checkSphereWithAABB(const Vector3f& center_A_, float radius_A_, const Vector3f& min_pt_B_, const Vector3f& max_pt_B_);
		static CollisionResult checkSphereWithOBB(const Vector3f& center_A_, float radius_A_, const Vector3f& center_B_, const Vector3f& size_B_, const Matrix3f& rotationMatrix_B_);
		static CollisionResult checkSphereWithPlane(const Vector3f& center_A_, float radius_A_, const Vector3f& normal_B_, float distance_B_);

		static CollisionResult checkTriangleWithSphere(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const Vector3f& center_B_, float radius_B_);
		static CollisionResult checkTriangleWithPlane(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const Vector3f& normal_B_, float distance_B_);
		static CollisionResult checkTriangleWithAABB(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const AABBShape* pAABB_B_, const BodyTransform* pTransform_B_);
		static CollisionResult checkTriangleWithOBB(const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, const OBBShape* pOBB_B_, const BodyTransform* pTransform_B_);
		static CollisionResult checkTriangleWithTriangle(const Vector3f& pt0_A_, const Vector3f& pt1_A_, const Vector3f& pt2_A_, const Vector3f& pt0_B_, const Vector3f& pt1_B_, const Vector3f& pt2_B_);

		static CollisionResult checkAABBWithAABB(const Vector3f& min_A_, const Vector3f& max_A_, const Vector3f& min_B_, const Vector3f& max_B_);

		static CollisionResult checkCapsuleWithAABB(const Vector3f& center_A_, const Vector3f& normal_A_, float height_A_, float radius_A_, const Vector3f& min_pt_B_, const Vector3f& max_pt_B_);
		static CollisionResult checkCapsuleWithCapsule(const Vector3f& center_A_, const Vector3f& normal_A_, float height_A_, float radius_A_, const Vector3f& center_B_, const Vector3f& normal_B_, float height_B_, float radius_B_);
		static CollisionResult checkCapsuleWithOBB(const Vector3f& center_A_, const Vector3f& normal_A_, float height_A_, float radius_A_, const Vector3f& center_B_, const Vector3f& size_B_, const Matrix3f& rotationMatrix_B_);
		static CollisionResult checkCapsuleWithPlane(const Vector3f& center_A_, const Vector3f& normal_A_, float height_A_, float radius_A_, const Vector3f& normal_B_, float distance_B_);
		static CollisionResult checkCapsuleWithSphere(const Vector3f& center_A_, const Vector3f& normal_A_, float height_A_, float radius_A_, const Vector3f& center_B_, float radius_B_);
		static CollisionResult checkCapsuleWithTriangle(const Vector3f& center_A_, const Vector3f& normal_A_, float height_A_, float radius_A_, const Vector3f& pt0_B_, const Vector3f& pt1_B_, const Vector3f& pt2_B_);

		// AABBShape* pAABB_A_, BodyTransform* pTransform_A_
	};
}