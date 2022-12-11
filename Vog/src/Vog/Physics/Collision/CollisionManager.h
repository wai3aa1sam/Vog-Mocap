#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Vog/Physics/Collision/CollisionResult.h"

#include <vector>

// References: 
// Physics for Game Developers, 2nd Edition
// https://en.wikipedia.org/wiki/Collision_response

namespace vog {

	class VOG_API CollisionManager : public NonCopyable
	{
	public:
		CollisionManager();
		~CollisionManager();

		void resolveCollision(float dt_, const Vector3f& gravity_);
		void applyImpulses();


		void linearProjection();

		void clear();

		void invokeCollisionExitCallback(Body& body_A_, Body& body_B_);

		bool isCollisionExist(BodyHandle handle_A_, BodyHandle handle_B_, uint32_t cantorPairing_);
		bool isCollisionExist(BodyHandle handle_A_, BodyHandle handle_B_);

		std::vector<CollisionResult> m_collisionResults;
		size_t m_collisionCount = 0;
		int m_impulseIteration = 5;
		float m_penetrationSlack = 0.001f;
		float m_linearProjectionPercent = 0.45f;
	private:
		uint32_t _cantorPairing(uint32_t x_, uint32_t y_);
		void _invokeCollisionCallback(Body& body_A_, Body& body_B_, bool isPreviousColliding_);
		void _removeCollisionPair(BodyHandle handle_A_, BodyHandle handle_B_);

	private:
		std::unordered_set<uint32_t> m_pairIDSet;
	};
}