#include "vogpch.h"
#include "PhysicsWorld.h"

#include "Vog/Physics/Collision/Collision.h"
#include "Vog/Physics/Collision/CollisionResult.h"

#include "Vog/Physics/Raycast/Raycast.h"

#include "Vog/ImGui/ImGuiLibrary.h"
#include <imgui.h>

namespace vog {
//#define ENABLE_octree

	PhysicsWorld::PhysicsWorld(Vector3f gravity_)
		:
		m_gravity(gravity_)
	{
		if (isUseGravity)
		{
			VOG_CORE_LOG_INFO("Gravity is using in Physics Wolrd!");
		}
		else
		{
			VOG_CORE_LOG_INFO("Warning: No Gravity is using in Physics Wolrd!");
		}

		m_bodyManager.reserve(100);
		m_collisionManager.m_collisionResults.reserve(100);
	}

	PhysicsWorld::~PhysicsWorld()
	{
		destroy();
	}

	void PhysicsWorld::destroy()
	{
		m_bodyManager.destroy();
		m_collisionManager.m_collisionResults.clear();
	}

	void PhysicsWorld::onStart()
	{
#ifdef ENABLE_octree
		for (size_t i = 0; i < m_bodyPtrs.size(); i++)
		{
			m_root.insert(&m_root, m_bodyPtrs[i]->m_id, m_bodyPtrs);
		}
		m_root.splitTree(m_bodyPtrs);
#endif // ENABLE_octree
	}

	void PhysicsWorld::step(float dt_)
	{
		float dt = dt_;

		//checkRaycast();

		// check collision
		// apply force
		// apply impulse for collisions
		// Integrate the force and velocity 
		// perform Linear Projection to fix any sinking issues

		m_bodyManager.updateProperties();
		m_bodyManager.checkCollision(m_collisionManager);

#ifdef ENABLE_octree
		m_root.update(m_bodyPtrs);
#endif // ENABLE_octree

		applyForces();

		m_collisionManager.resolveCollision(dt_, m_gravity);
		//solveColision(dt_);

		// solver (integrate)
		solver(dt);
		//integrate(dt_);

		//m_collisionManager.linearProjection();

		m_collisionManager.clear();
		// clear all Forces
		clearForces();

		m_bodyManager.updateHandles();
	}

	Body* PhysicsWorld::addBody()
	{
		return m_bodyManager.addBody();
	}

	void PhysicsWorld::removeBody(BodyHandle handle_)
	{
		m_bodyManager.removeBody_request(handle_);
	}

	void PhysicsWorld::raycast(const Vector3f origin_, const Vector3f direction_)
	{
		Ray ray = { origin_, direction_ };
		for (size_t j = 0; j < m_bodyManager.getSize(); j++)
		{
			auto* pShape = m_bodyManager.getBodies()[j]->m_pShape;
			auto& transform = m_bodyManager.getBodies()[j]->m_transform;

			bool isHitted = false;
			RaycastResult ret;

			if (pShape->getType() == ShapeType::AABB)
			{
				isHitted = Raycast::isHitted(ray, (const AABBShape*)pShape, &transform, ret);
			}
			else if (pShape->getType() == ShapeType::OBB)
			{
				isHitted = Raycast::isHitted(ray, (const OBBShape*)pShape, &transform, ret);
			}
			else if (pShape->getType() == ShapeType::Plane)
			{
				isHitted = Raycast::isHitted(ray, (const PlaneShape*)pShape, &transform, ret);
			}
			else if (pShape->getType() == ShapeType::Triangle)
			{
				isHitted = Raycast::isHitted(ray, (const TriangleShape*)pShape, &transform, ret);
			}
			else if (pShape->getType() == ShapeType::Sphere)
			{
				isHitted = Raycast::isHitted(ray, (const SphereShape*)pShape, &transform, ret);
			}
			
			if (isHitted)
			{
				VOG_CORE_LOG_INFO("Raycast hitted {0}, t: {1}", pShape->getName(), ret.t);
			}
			else
			{
				//VOG_CORE_LOG_INFO("No raycast hitted");
			}
		}
	}

	void PhysicsWorld::onImGuiRender()
	{
		ImGui::Begin("Physics Debug Settings");

		ImGuiLibrary::drawTextWithValue("Object count", m_bodyManager.getSize());
		ImGuiLibrary::drawTextWithValue("Collision count", m_collisionManager.m_collisionCount);

		ImGuiLibrary::drawDragFloat("linear Projection Percent", m_collisionManager.m_linearProjectionPercent);
		ImGuiLibrary::drawDragFloat("penetration Slack", m_collisionManager.m_penetrationSlack);
		ImGuiLibrary::drawInputInt("impulse Iteration", m_collisionManager.m_impulseIteration);
		ImGuiLibrary::drawCheckbox("is use gravity", isUseGravity); 
		ImGuiLibrary::drawDragFloat3("gravity", m_gravity);

		ImGui::End();
	}

	void PhysicsWorld::checkRaycast()
	{
		for (size_t i = 0; i < m_rays.size(); i++)
		{
			for (size_t j = 0; j < m_bodyManager.getBodies().size(); j++)
			{
				auto* pShape = m_bodyManager.getBodies()[j]->m_pShape;
				auto& transform = m_bodyManager.getBodies()[j]->m_transform;

				bool isHitted = false;
				RaycastResult ret;

				if (pShape->getType() == ShapeType::AABB)
				{
					isHitted = Raycast::isHitted(m_rays[i], (const AABBShape*)pShape, &transform, ret);
				}
				else if (pShape->getType() == ShapeType::OBB)
				{
					isHitted = Raycast::isHitted(m_rays[i], (const OBBShape*)pShape, &transform, ret);
				}
				else if (pShape->getType() == ShapeType::Plane)
				{
					isHitted = Raycast::isHitted(m_rays[i], (const PlaneShape*)pShape, &transform, ret);
				}
				else if (pShape->getType() == ShapeType::Sphere)
				{
					isHitted = Raycast::isHitted(m_rays[i], (const SphereShape*)pShape, &transform, ret);
				}

				if (isHitted)
				{
					VOG_CORE_LOG_INFO("Raycast hitted {0}, t: {1}", pShape->getName(), ret.t);
				}
				else
				{
					VOG_CORE_LOG_INFO("No raycast hitted");
				}
			}
		}

		m_rays.clear();
	}

	void PhysicsWorld::checkCollision()
	{
		/*for (size_t i = 0; i < m_bodyPtrs.size(); i++)
		{
			for (size_t j = i + 1; j < m_bodyPtrs.size(); j++)
			{
				if (!m_bodyPtrs[i]->m_pShape || !m_bodyPtrs[j]->m_pShape)
					continue;

				auto* pShape_A = m_bodyPtrs[i]->m_pShape;
				auto* pShape_B = m_bodyPtrs[j]->m_pShape;

				auto& transform_A = m_bodyPtrs[i]->m_transform;
				auto& transform_B = m_bodyPtrs[j]->m_transform;

				CollisionResult ret = pShape_A->collide(&transform_A, pShape_B, &transform_B);

				if (ret.isCollided)
				{
					ret.pBody_A = m_bodyPtrs[i];
					ret.pBody_B = m_bodyPtrs[j];

					m_collisionManager.m_collisionResults.emplace_back(ret);
				}
			}
		}*/

		//m_collisionManager.m_collisionResults = m_root.checkCollision(m_bodyPtrs);
	}

	void PhysicsWorld::applyForces()
	{
		float LINEARDRAGCOEFFICIENT = 0.5f;
		float ANGULARDRAGCOEFFICIENT = 0.5f;

		if (isUseGravity)
		{
			for (auto* pBody : m_bodyManager.getBodies())
			{
				auto& data = pBody->m_data;
				auto& transform = pBody->m_transform;

				if (pBody->m_type == BodyType::Static)
					continue;

				// drag force
				// Fd = 0.5 * C * density * Area * velocity^2 
				Vector3f dir_drag = -data.velocity_local;
				dir_drag = MyMath::normalize(dir_drag);
				float speed = data.speed;
				float density_air = 1.225f;
				float area = 1.5f;
				Vector3f force = dir_drag * (0.5f * LINEARDRAGCOEFFICIENT * density_air * area * speed * speed);

				Vector3f dir_angular_drag = -data.angularVelocity_local;
				dir_angular_drag = MyMath::normalize(dir_angular_drag);
				float angularSpeed = MyMath::magnitude(data.angularVelocity_local);
				// area = 
				Vector3f torque = dir_angular_drag * (0.5f * LINEARDRAGCOEFFICIENT * density_air * area * angularSpeed * angularSpeed);

				//data.force = transform.orientation * force;
				//data.torque += torque;

				data.force += data.mass * m_gravity;
			}
		}
	}

	void PhysicsWorld::solver(float dt_)
	{
		for (auto* pBody : m_bodyManager.getBodies())
		{
			if (pBody->m_type == BodyType::Static)
				continue;

			auto& data = pBody->m_data;
			auto& transform = pBody->m_transform;

			Vector3f F;			// total force
			Vector3f A;			// acceleration
			Vector3f Vnew;		// new velocity at time t + dt
			Vector3f Snew;		// new position at time t + dt
			Vector3f k1, k2;

			F = data.force;
			A = F / data.mass;

			k1 = dt_ * A;
			k2 = dt_ * A;

			Vnew = data.velocity + (k1 + k2) / 2.0f;
			
			const float damping = 0.98f;

			data.velocity = Vnew * damping;

			Snew = transform.position + Vnew * dt_;

			transform.position = Snew;

			// rotation
			//{
			//	auto aungularAcceleration = data.inertiaInverse * (data.torque - (MyMath::cross(data.angularVelocity, (data.inertia * data.angularVelocity))));
			//	data.angularVelocity += aungularAcceleration * dt_;
			//	Quaternion dq = transform.orientation * data.angularVelocity * 0.5f * dt_;			// dp/dt = 0.5 * omega * q
			//	transform.orientation += dq;

			//	float magnitude = MyMath::magnitude(dq);
			//	if (magnitude > 0.0001f)
			//		transform.orientation /= magnitude;
			//		//MyMath::normalize(transform.orientation);

			//	data.velocity_local = MyMath::conjugate(transform.orientation) * data.velocity;		// to local space
			//}

			data.speed = MyMath::magnitude(data.velocity);
		}
	}

	void PhysicsWorld::clearForces()
	{
		for (auto* pBody : m_bodyManager.getBodies())
		{
			auto& data = pBody->m_data;
			data.debug_force = data.force;
			data.force = Vector3f(0.0f, 0.0f, 0.0f);
		}
	}
	
	void PhysicsWorld::integrate(float dt_)
	{
		Vector3f acceleration;
		for (auto* pBody : m_bodyManager.getBodies())
		{
			if (pBody->m_type == BodyType::Static)
				continue;

			auto& data = pBody->m_data;
			auto& transform = pBody->m_transform;

			// Calculate the acceleration in earth space:
			float mass_inv = 1.0f / data.mass;
			acceleration = data.force * mass_inv;
			data.acceleration = acceleration;
			data.velocity += data.acceleration * dt_;
			transform.position += data.velocity * dt_;

			float mag;
			data.angluarAcceleration_local = data.inertiaInverse * (data.torque - (MyMath::cross(data.angularVelocity, (data.inertia * data.angularVelocity))));
			data.angularVelocity_local += data.angluarAcceleration_local * dt_;

			Quaternion dq = (transform.orientation * data.angularVelocity) * 0.5f * dt_;			// dp/dt = 0.5 * omega * q
			transform.orientation += dq;

			mag = MyMath::magnitude(transform.orientation);
			if (!MyMath::isEqualTo(mag, 0.0f))
				transform.orientation /= mag;

			// Calculate the velocity in body space:
			data.velocity_local = MyMath::conjugate(transform.orientation) * data.velocity;

			// Get the angular velocity in global coords:
			data.angularVelocity = transform.orientation * data.angularVelocity_local;

			// Get the angular acceleration in global coords:
			data.angluarAcceleration = transform.orientation * data.angluarAcceleration_local;

			Matrix3f r, rt;
			r = MyMath::toMatrix3f(transform.orientation);
			rt = MyMath::transpose(r);
			data.inertiaInverse = r * data.inertiaInverse_local * rt;

			data.speed = MyMath::magnitude(data.velocity);
		}
	}
}