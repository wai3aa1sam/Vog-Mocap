#include "vogpch.h"
#include "Particle.h"

#include "Vog/Math/MyRandom.h"

namespace vog::Test {
	Particle::Particle()
	{
		position = MyRandom::Vec3f(0.0f, 1.0f);
		velocity = MyRandom::Vec3f(0.0f, 1.0f);
	}

	void Particle::update(float dt_, const Vector3f& gravity_)
	{
		force += mass * gravity_;

		Vector3f acceleration = { 0.0f, 0.0f, 0.0f };
		Vector3f dv = { 0.0f, 0.0f, 0.0f };
		Vector3f ds = { 0.0f, 0.0f, 0.0f };

		acceleration = force / mass;

		dv = acceleration * dt_;
		velocity += dv;

		ds = velocity * dt_;
		position += ds;

		speed = static_cast<float>(velocity.length());
	}

	ParticleSystem::ParticleSystem(int count_, const Vector3f& gravity_)
		:
		gravity(gravity_)
	{
		m_pParticlesBuffer = new Particle[count_];
		m_itParticlesBuffer = m_pParticlesBuffer;
		m_particlesCount = count_;
		m_currentParticlesCount = 0;
	}

	void ParticleSystem::step(float dt_)
	{
		m_itParticlesBuffer = m_pParticlesBuffer;
		auto* end = m_itParticlesBuffer + m_currentParticlesCount;
		VOG_CORE_ASSERT(m_itParticlesBuffer + m_currentParticlesCount <= m_pParticlesBuffer + m_particlesCount && m_itParticlesBuffer >= m_pParticlesBuffer, "");

		for (size_t i = 0; i < m_particlesCount; i++)
		{
			m_itParticlesBuffer[i].update(dt_, gravity);
		}
	}
}