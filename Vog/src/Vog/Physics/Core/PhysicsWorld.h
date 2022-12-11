#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Vog/Physics/Body/BodyManager.h"

#include "Vog/Physics/Collision/CollisionManager.h"

#include "Vog/Physics/Raycast/Ray.h"

#include "Vog/Physics/BoardPhase/Octree.h"

#include <vector>

// References:
// Physics for Game Developers, 2nd Edition
// References: Game Physics Cookbook (2017)
// https://blog.winter.dev/2020/designing-a-physics-engine/

namespace vog {

	class VOG_API PhysicsWorld : public NonCopyable
	{
	public:
		static constexpr int s_max_particles_count = 1000;
	public:
		PhysicsWorld(Vector3f gravity_ = {0.0f, -9.81f, 0.0f});
		~PhysicsWorld();

		void destroy();

		void onStart();

		void init();
		void step(float dt_);

		Body* addBody();
		void removeBody(BodyHandle handle_);

		void raycast(const Vector3f origin_, const Vector3f direction_);

		void onImGuiRender();

		bool isUseGravity = false;

	private:
		//void apply
		void checkRaycast();
		void checkCollision();
		void applyForces();
		void solver(float dt_);

		void clearForces();

		// new
		void integrate(float dt_);

	private:
		Vector3f m_gravity = { 0.0f, 0.0f, 0.0f };

		//std::vector<Body*> m_bodyPtrs;
		BodyManager m_bodyManager;
		OctreeNode m_root;

		CollisionManager m_collisionManager;

		std::vector<Ray> m_rays;

		//-------
		
	};
}