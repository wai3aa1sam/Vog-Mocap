#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

namespace vog::Test {

	class VOG_API Particle : public NonCopyable
	{
	public:
		Particle();
		void update(float dt_, const Vector3f& gravity_);
		//void render

	public:
		Vector3f position = { 0.0f, 0.0f, 0.0f };
		Vector3f velocity = {0.0f, 0.0f, 0.0f};
		float mass = 1;

		Vector3f force = { 0.0f, 0.0f, 0.0f };
		float radius = 0.0f; // Particle radius used for collision detection
		float speed = 0.0f;
	};

	class VOG_API ParticleSystem : public NonCopyable
	{
	public:
		ParticleSystem(int count_, const Vector3f& gravity_ = Vector3f(0.0f, -0.000000981f, 0.0f));
		void step(float dt_);
		//void render
		~ParticleSystem()
		{
			delete[] m_pParticlesBuffer;
			m_pParticlesBuffer = nullptr;
			m_itParticlesBuffer = nullptr;
		}
	public:
		int m_particlesCount = 0;
		int m_currentParticlesCount = 0;

		Particle* m_pParticlesBuffer = nullptr;
		Particle* m_itParticlesBuffer = nullptr;

		Vector3f gravity;

	};
}