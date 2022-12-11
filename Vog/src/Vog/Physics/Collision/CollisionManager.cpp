#include "vogpch.h"
#include "CollisionManager.h"

namespace vog {
	CollisionManager::CollisionManager()
	{
		m_pairIDSet.reserve(1000);
	}

	CollisionManager::~CollisionManager()
	{
	}

	void CollisionManager::resolveCollision(float dt_, const Vector3f& gravity_)
	{
		//for (size_t iteration = 0; iteration < m_impulseIteration; iteration++)
		{
			for (size_t i = 0; i < m_collisionResults.size(); i++)
			{
				VOG_CORE_ASSERT(m_collisionResults[i].pBody_A, "");
				VOG_CORE_ASSERT(m_collisionResults[i].pBody_B, "");

				//VOG_CORE_LOG_INFO("Collide!");

				auto& ret = m_collisionResults[i];

				auto& body_A = *ret.pBody_A;
				auto& body_B = *ret.pBody_B;

				auto& data_A = body_A.m_data;
				auto& data_B = body_B.m_data;

				auto* pShape_A = body_A.m_pShape;
				auto* pShape_B = body_B.m_pShape;

				if (!pShape_A || !pShape_B)
				{
					continue;
				}

				uint32_t pair = _cantorPairing(body_A.m_handle, body_B.m_handle);
				bool isPreviousCollidng = isCollisionExist(body_A.m_handle, body_B.m_handle, pair);
				if (!isPreviousCollidng)
				{
					m_pairIDSet.insert(pair);
					m_pairIDSet.insert(_cantorPairing(body_B.m_handle, body_A.m_handle));
				}

				_invokeCollisionCallback(body_A, body_B, isPreviousCollidng);

#ifdef VOG_ENABLE_SHAPE_NAME
				//VOG_CORE_LOG_INFO("{0} Collided with {1}! direction: <{2}, {3}, {4}>, depth: {5}", 
				//	pShape_A->m_debug_name, pShape_B->m_debug_name,
				//	ret.direction.x, ret.direction.y, ret.direction.z, ret.penetration_depth);
#endif // VOG_ENABLE_SHAPE_NAME

				continue;		// TODO: remove, just use to block the collision response!

				auto& transform_A = body_A.m_transform;
				auto& transform_B = body_B.m_transform;

				if (body_A.m_type == BodyType::Dynamic)
				{
					transform_A.position -= ret.direction * ret.penetration_depth;
				}

				if (body_B.m_type == BodyType::Dynamic)
				{
					transform_B.position += ret.direction * ret.penetration_depth;
				}

				//continue;		// TODO: remove, just use to block the collision response!

				Vector3f relative_velocity = data_B.velocity - data_A.velocity;
				Vector3f relative_normal = ret.direction;

				float proj_normal = MyMath::dot(relative_velocity, relative_normal);

				if (proj_normal > 0.0f)		// is not towards together
					continue;

				float restitution = MyMath::min(data_A.restitution, data_B.restitution);

				float inv_mass_A = 1.0f / (data_A.mass + MyMath::epsilon);
				float inv_mass_B = 1.0f / (data_B.mass + MyMath::epsilon);
				float sum_of_inv_mass = inv_mass_A + inv_mass_B;

				float impluse_mag_numer = -proj_normal * (restitution + 1.0f);
				float impluse_mag_denom = (inv_mass_A + inv_mass_B);
				float impulse_mag = (MyMath::isEqualTo(impluse_mag_denom, 0.0f)) ? 0.0f : impluse_mag_numer / impluse_mag_denom;

				Vector3f impulse = relative_normal * impulse_mag;
				data_A.velocity -= impulse * inv_mass_A;
				data_B.velocity += impulse * inv_mass_B;

				// friction
				Vector3f tangent_velocity = relative_velocity - (relative_normal * proj_normal);
				float tangent_velocity_mag_sq = MyMath::magnitude_square(tangent_velocity);
				if (MyMath::isEqualTo(tangent_velocity_mag_sq, 0.0f))
					continue;

				tangent_velocity /= MyMath::sqrt(tangent_velocity_mag_sq);

				impluse_mag_numer = -MyMath::dot(relative_velocity, tangent_velocity);
				float tangent_impluse_mag = impluse_mag_numer / impluse_mag_denom;
				if (MyMath::isEqualTo(tangent_impluse_mag, 0.0f))
					continue;

				float friction = MyMath::sqrt(data_A.friction * data_B.friction);
				if (tangent_impluse_mag > impulse_mag * friction)
					tangent_impluse_mag = impulse_mag * friction;
				else if (tangent_impluse_mag < -impulse_mag * friction)
					tangent_impluse_mag = -impulse_mag * friction;

				Vector3f tangent_impluse = tangent_velocity * tangent_impluse_mag;
				data_A.velocity -= tangent_impluse * inv_mass_A;
				data_B.velocity += tangent_impluse * inv_mass_B;

				// Impulse-based reaction model
				if (ret.contact_points.size() > 0)
				{
					for (size_t i = 0; i < ret.contact_points.size(); i++)
					{
						Vector3f dir_A_to_contact = ret.contact_points[i] - transform_A.position;
						Vector3f dir_B_to_contact = ret.contact_points[i] - transform_B.position;

						relative_velocity = (data_B.velocity + MyMath::cross(data_B.angularVelocity, dir_B_to_contact))
							- (data_A.velocity + MyMath::cross(data_A.angularVelocity, dir_A_to_contact));


						proj_normal = MyMath::dot(relative_velocity, relative_normal);
						if (proj_normal > 0.0f)		// is not towards together
							continue;

						// w' = w + j * I^(-1) * (r x n)
						Vector3f impulse_A = MyMath::cross(data_A.inertiaInverse * MyMath::cross(dir_A_to_contact, relative_normal), dir_A_to_contact);
						Vector3f impulse_B = MyMath::cross(data_B.inertiaInverse * MyMath::cross(dir_B_to_contact, relative_normal), dir_B_to_contact);

						// j = (-(1 + e) * vr * n) / (1 / m1 + 1 / m2 + (I1(-1) * (r1 x n) x r1 + I2(-1) * (r2 x n) x r2 ) * n)
						// j: impluse_magnitude
						float impluse_mag_numer = -(1.0f + restitution) * MyMath::dot(relative_velocity, relative_normal);
						float impluse_mag_denom = sum_of_inv_mass + MyMath::dot(impulse_A + impulse_B, ret.direction);
						float impulse_mag = (MyMath::isEqualTo(impluse_mag_denom, 0.0f)) ? 0.0f : impluse_mag_numer / impluse_mag_denom;

						Vector3f impulse = relative_normal * impulse_mag;

						data_A.angularVelocity -= data_A.inertia * MyMath::cross(dir_A_to_contact, impulse);
						data_B.angularVelocity += data_B.inertia * MyMath::cross(dir_B_to_contact, impulse);

						data_A.torque -= MyMath::cross(ret.contact_points[i] - transform_A.position, impulse);
						data_B.torque += MyMath::cross(ret.contact_points[i] - transform_B.position, impulse);

						// friction
						Vector3f tangent_velocity = relative_velocity - (relative_normal * proj_normal);
						float tangent_velocity_mag_sq = MyMath::magnitude_square(tangent_velocity);
						if (MyMath::isEqualTo(tangent_velocity_mag_sq, 0.0f))
							continue;

						tangent_velocity /= MyMath::sqrt(tangent_velocity_mag_sq);

						impluse_mag_numer = -MyMath::dot(relative_velocity, tangent_velocity);

						impulse_A = MyMath::cross(data_A.inertiaInverse * MyMath::cross(dir_A_to_contact, tangent_velocity), dir_A_to_contact);
						impulse_B = MyMath::cross(data_B.inertiaInverse * MyMath::cross(dir_B_to_contact, tangent_velocity), dir_B_to_contact);

						impluse_mag_denom = sum_of_inv_mass + MyMath::dot(tangent_velocity, impulse_A + impulse_B);
						if (MyMath::isEqualTo(impluse_mag_denom, 0.0f))
							continue;

						float tangent_impluse_mag = impluse_mag_numer / impluse_mag_denom;
						if (MyMath::isEqualTo(tangent_impluse_mag, 0.0f))
							continue;
						tangent_impluse_mag /= ret.contact_points.size();

						float friction = MyMath::sqrt(data_A.friction * data_B.friction);
						if (tangent_impluse_mag > impulse_mag * friction)
							tangent_impluse_mag = impulse_mag * friction;
						else if (tangent_impluse_mag < -impulse_mag * friction)
							tangent_impluse_mag = -impulse_mag * friction;

						Vector3f tangent_impluse = tangent_velocity * tangent_impluse_mag;
						data_A.velocity -= tangent_impluse * inv_mass_A;
						data_B.velocity += tangent_impluse * inv_mass_B;

						data_A.angularVelocity -= data_A.inertia * MyMath::cross(dir_A_to_contact, tangent_impluse);
						data_B.angularVelocity += data_B.inertia * MyMath::cross(dir_B_to_contact, tangent_impluse);
					}
				}
			}
		}
	}

	void CollisionManager::applyImpulses()
	{
		for (int k = 0; k < m_impulseIteration; ++k)
		{
			for (size_t i = 0; i < m_collisionResults.size(); i++)
			{
				//for (size_t i = 0; i < ret.contact_points.size(); i++)

			}
		}
		
	}
	void CollisionManager::_invokeCollisionCallback(Body& body_A_, Body& body_B_, bool isPreviousColliding_)
	{
		if (body_A_.m_pCollisionCallbackBase)
		{
			VOG_CORE_ASSERT(body_A_.m_pCollisionCallbackBase, "");
			if (isPreviousColliding_)
				body_A_.m_pCollisionCallbackBase->onTriggerStay(body_B_.m_userHandle);
			else
				body_A_.m_pCollisionCallbackBase->onTriggerEnter(body_B_.m_userHandle);
		}

		if (body_B_.m_pCollisionCallbackBase)
		{
			VOG_CORE_ASSERT(body_B_.m_pCollisionCallbackBase, "");
			if (isPreviousColliding_)
				body_B_.m_pCollisionCallbackBase->onTriggerStay(body_A_.m_userHandle);
			else
				body_B_.m_pCollisionCallbackBase->onTriggerEnter(body_A_.m_userHandle);
		}
	}

	void CollisionManager::_removeCollisionPair(BodyHandle handle_A_, BodyHandle handle_B_)
	{
		auto cantorPairing_ = _cantorPairing(handle_A_, handle_B_);
		m_pairIDSet.extract(cantorPairing_);
		cantorPairing_ = _cantorPairing(handle_B_, handle_A_);
		m_pairIDSet.extract(cantorPairing_);
	}

	void CollisionManager::linearProjection()
	{
		for (size_t i = 0; i < m_collisionResults.size(); i++)
		{
			auto& ret = m_collisionResults[i];

			auto& config = ret.config;
			if (config.isTrigger)
			{
				continue;
			}

			auto& body_A = *ret.pBody_A;
			auto& body_B = *ret.pBody_B;

			auto& data_A = body_A.m_data;
			auto& data_B = body_B.m_data;

			auto* pShape_A = body_A.m_pShape;
			auto* pShape_B = body_B.m_pShape;

			if (!pShape_A || !pShape_B)
			{
				continue;
			}

			auto& transform_A = body_A.m_transform;
			auto& transform_B = body_B.m_transform;

			float inv_mass_A = 1.0f / (data_A.mass + MyMath::epsilon);
			float inv_mass_B = 1.0f / (data_B.mass + MyMath::epsilon);
			float sum_of_inv_mass = inv_mass_A + inv_mass_B;

			if (MyMath::isEqualTo(sum_of_inv_mass, 0.0f))
				continue;

			float depth = MyMath::max(ret.penetration_depth - m_penetrationSlack, 0.0f);
			float scalar = depth / sum_of_inv_mass;
			Vector3f correction = ret.direction * scalar * m_linearProjectionPercent;

			if (body_A.m_type == BodyType::Dynamic)
				transform_A.position -= correction * inv_mass_A /** (float)body_A.m_type*/;

			if (body_B.m_type == BodyType::Dynamic)
				transform_B.position += correction * inv_mass_B /** (float)body_B.m_type*/;
		}
	}
	void CollisionManager::clear()
	{
		m_collisionCount = m_collisionResults.size();
		// remove all collision result
		m_collisionResults.clear();
	}

	void CollisionManager::invokeCollisionExitCallback(Body& body_A_, Body& body_B_)
	{
		if (body_A_.m_pCollisionCallbackBase)
		{
			body_A_.m_pCollisionCallbackBase->onTriggerExit(body_B_.m_userHandle);
		}

		if (body_B_.m_pCollisionCallbackBase)
		{
			body_B_.m_pCollisionCallbackBase->onTriggerExit(body_A_.m_userHandle);
		}
		_removeCollisionPair(body_A_.m_handle, body_B_.m_handle);
	}

	bool CollisionManager::isCollisionExist(BodyHandle handle_A_, BodyHandle handle_B_, uint32_t cantorPairing_)
	{
		auto it = m_pairIDSet.find(cantorPairing_);
		if (it != m_pairIDSet.end())
		{
			return true;
		}
		else
		{
			cantorPairing_ = _cantorPairing(handle_B_, handle_A_);
			return m_pairIDSet.find(cantorPairing_) != m_pairIDSet.end();
		}
	}

	bool CollisionManager::isCollisionExist(BodyHandle handle_A_, BodyHandle handle_B_)
	{
		auto cantorPairing_ = _cantorPairing(handle_A_, handle_B_);
		auto it = m_pairIDSet.find(cantorPairing_);
		if (it != m_pairIDSet.end())
		{
			return true;
		}
		else
		{
			cantorPairing_ = _cantorPairing(handle_B_, handle_A_);
			return m_pairIDSet.find(cantorPairing_) != m_pairIDSet.end();
		}
	}

	uint32_t CollisionManager::_cantorPairing(uint32_t x_, uint32_t y_)
	{
		//VOG_CORE_ASSERT(x_ > 0 && y_ > 0, "");
		return (x_ * x_ + 3 * x_ + 2 * x_ * y_ + y_ + y_ * y_) / 2;
	}
}