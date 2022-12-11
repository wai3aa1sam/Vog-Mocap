#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

namespace vog {
	enum class BodyType
	{
		Static = 0, Dynamic, Kinematic,
	};

	struct BodyData
	{
		Matrix3f inertia				= Matrix3f{ 1.0f };										// mass moment of inertia
		Matrix3f inertiaInverse_local	= Matrix3f{ 1.0f };									// inverse of mass moment of inertia
		Matrix3f inertiaInverse			= Matrix3f{ 1.0f };									// inverse of mass moment of inertia

		Vector3f velocity			= Vector3f{ 0.0f, 0.0f, 0.0f };					// velocity in gobal coordinates
		Vector3f velocity_local = Vector3f{ 0.0f, 0.0f, 0.0f };						// velocity in body coordinates

		Vector3f angularVelocity	= Vector3f{ 0.0f, 0.0f, 0.0f };					// angular velocity in gobal coordinates
		Vector3f angularVelocity_local	= Vector3f{ 0.0f, 0.0f, 0.0f };				// angular velocity in body coordinates

		Vector3f acceleration = Vector3f{ 0.0f, 0.0f, 0.0f };
		Vector3f angluarAcceleration_local = Vector3f{ 0.0f, 0.0f, 0.0f };
		Vector3f angluarAcceleration = Vector3f{ 0.0f, 0.0f, 0.0f };

		Vector3f force = Vector3f{ 0.0f, 0.0f, 0.0f };								// total force on body
		Vector3f torque = Vector3f{ 0.0f, 0.0f, 0.0f };								// total moment (torque) on body

		float speed = 0.0f;															// speed (magnitude of the velocity)
		float mass = 1.0f;															// total mass
		float restitution = 0.1f;
		float friction = 0.2f;

		// Debug
		Vector3f debug_force = Vector3f{ 0.0f, 0.0f, 0.0f }; // total force on body
	};
}

