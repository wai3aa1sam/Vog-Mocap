#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Particle.h"

namespace vog::Test {

	class VOG_API PhysicsEngine : public NonCopyable
	{
	public:
		static constexpr int s_max_particles_count = 1000;
	public:
		PhysicsEngine(Vector3f gravity_ = { 0.0f, 0.0f, 0.0f });
		~PhysicsEngine();

		void destroy();

		void init();
		void step(float dt_);

		ParticleSystem m_particleSystem;


	private:
		Vector3f m_gravity = { 0.0f, 0.0f, 0.0f };

	};
}