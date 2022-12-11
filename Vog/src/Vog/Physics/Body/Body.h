#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Vog/Physics/Body/BodyData.h"
#include "Vog/Physics/Body/BodyTransform.h"

#include "Vog/Physics/Collision/Shapes/Shape.h"

#include "Vog/Physics/Collision/CollisionCallbackBase.h"

#include <vector>

namespace vog {

	using BodyHandle = uint32_t;
	static const BodyHandle s_invalid_body = 0;

	class VOG_API Body
	{
	public:
		friend class PhysicsWorld;
	public:
		Body();
		~Body();

		void destroy();

		void init();

		void update(float dt_);

		Shape* setAsBox(const Vector3f& center_, const Vector3f& size_, const Vector3f& rotation_);
		//Shape* setAsCapsule(const Vector3f& base_, const Vector3f& normal_, float height_, float radius_);
		Shape* setAsCapsule(const Vector3f& base_, const Vector3f& tip_, float height_, float radius_);

		Shape* setAsSphere(const Vector3f& center_, float radius_);
		Shape* setAsPlane(const Matrix4f& transform_);

		void addRotationalImpulse(const Vector3f& point_, const Vector3f& impulse_, float dt_);

		BodyHandle getHandle() { return m_handle; }

	public:		// TODO: change to private
		BodyHandle m_handle = s_invalid_body;

		BodyTransform m_transform;
		BodyData m_data;
		BodyType m_type = BodyType::Static;

		bool isTrigger = false;
		bool isGravityUsed = true;

		Shape* m_pShape = nullptr;

		//void* m_pUserData = nullptr;
		uint32_t m_userHandle = 0;		// TODO: redesign
		bool m_isEnable = true;

		// TODO: temporary collision callback 
		// should have id / collision id to perform the trigger stay and exit and pass his pair to the function!
		// maybe change to listener pattern?
		CollisionCallbackBase* m_pCollisionCallbackBase = nullptr;
		/*bool m_isColliding = false;
		bool m_isCollisionExited = false;*/
	};
}