#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Vog/Physics/Raycast/Ray.h"
#include "Vog/Physics/Raycast/RaycastResult.h"

// References: Game Physics Cookbook (2017)

namespace vog {

	class AABBShape;
	class SphereShape;
	class OBBShape;
	class PlaneShape;

	struct BodyTransform;

	class VOG_API Raycast : public NonCopyable
	{
	public:
		static bool isHitted(const Ray& ray_, const SphereShape* pSphere_,		const BodyTransform* pTransform_, RaycastResult& result_);
		static bool isHitted(const Ray& ray_, const AABBShape* pAABB_,			const BodyTransform* pTransform_, RaycastResult& result_);
		static bool isHitted(const Ray& ray_, const OBBShape* pOBB_,			const BodyTransform* pTransform_, RaycastResult& result_);
		static bool isHitted(const Ray& ray_, const PlaneShape* pPlane_,		const BodyTransform* pTransform_, RaycastResult& result_);
		static bool isHitted(const Ray& ray_, const TriangleShape* pTriangle_,	const BodyTransform* pTransform_, RaycastResult& result_);

		static bool isHittedSphere(const Ray& ray_, const Vector3f& center_, float radius_, RaycastResult& result_);
		static bool isHittedAABB(const Ray& ray_, const Vector3f& min_pt_, const Vector3f& max_pt_, RaycastResult& result_);
		static bool isHittedOBB(const Ray& ray_, const Vector3f& center_, const Vector3f& size_, const Matrix3f& rotationMatrix_, RaycastResult& result_);
		static bool isHittedPlane(const Ray& ray_, const Vector3f& normal_, float distance_, RaycastResult& result_);
		static bool isHittedTriangle(const Ray& ray_, const Vector3f& pt0_, const Vector3f& pt1_, const Vector3f& pt2_, RaycastResult& result_);
	};
}