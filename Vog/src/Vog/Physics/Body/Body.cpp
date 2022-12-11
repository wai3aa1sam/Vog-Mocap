#include "vogpch.h"
#include "Body.h"

#include "Vog/Physics/Collision/Shapes/CapsuleShape.h"
#include "Vog/Physics/Collision/Shapes/SphereShape.h"
#include "Vog/Physics/Collision/Shapes/PlaneShape.h"
#include "Vog/Physics/Collision/Shapes/AABBShape.h"
#include "Vog/Physics/Collision/Shapes/OBBShape.h"
#include "Vog/Physics/Collision/Shapes/TriangleShape.h"

namespace vog {
	Body::Body()
	{
	}
	Body::~Body()
	{
		delete m_pShape;
	}

	void Body::update(float dt_)
	{
		const float damping = 0.98f;
		Vector3f acceleration = m_data.force * (1.0f / m_data.mass);
		m_data.velocity += acceleration * dt_;
		m_data.velocity *= damping;

		Vector3f angAccel = m_data.inertiaInverse * m_data.torque;
		m_data.angularVelocity += angAccel * dt_;
		m_data.angularVelocity *= damping;

		m_transform.position += m_data.velocity * dt_;
		m_transform.orientation += MyMath::toQuaternion(m_data.angularVelocity * dt_);
	}

	Shape* Body::setAsSphere(const Vector3f& center_, float radius_)
	{
		Shape* pSphere = new SphereShape(center_, radius_);
		
		float Ixx = 0, Iyy = 0, Izz = 0;
		float Ixy = 0, Ixz = 0, Iyz = 0;

		Ixx = Iyy = Izz = (2.0f / 5.0f) * m_data.mass * radius_ * radius_;

		m_data.inertia[0][0] =  Ixx;
		m_data.inertia[0][1] = -Ixy;
		m_data.inertia[0][2] = -Ixz;
		m_data.inertia[1][0] = -Ixy;
		m_data.inertia[1][1] =  Iyy;
		m_data.inertia[1][2] = -Iyz;
		m_data.inertia[2][0] = -Ixz;
		m_data.inertia[2][1] = -Iyz;
		m_data.inertia[2][2] =  Izz;

		m_data.inertiaInverse_local = MyMath::inverse(m_data.inertia);
		m_pShape = pSphere;
		return pSphere;
	}
	Shape* Body::setAsBox(const Vector3f& center_, const Vector3f& size_, const Vector3f& rotation_)
	{
		Shape* pBox = new OBBShape(center_, size_, rotation_);

		float Ixx = 0, Iyy = 0, Izz = 0;
		float Ixy = 0, Ixz = 0, Iyz = 0;

		float factor = 1.0f / 12.0f;

		float x2 = size_.x * size_.x;
		float y2 = size_.y * size_.y;
		float z2 = size_.z * size_.z;

		Ixx = factor * m_data.mass * (y2 + z2);
		Iyy = factor * m_data.mass * (x2 + z2);
		Izz = factor * m_data.mass * (x2 + y2);

		m_data.inertia[0][0] = Ixx;
		m_data.inertia[0][1] = -Ixy;
		m_data.inertia[0][2] = -Ixz;
		m_data.inertia[1][0] = -Ixy;
		m_data.inertia[1][1] = Iyy;
		m_data.inertia[1][2] = -Iyz;
		m_data.inertia[2][0] = -Ixz;
		m_data.inertia[2][1] = -Iyz;
		m_data.inertia[2][2] = Izz;

		m_data.inertiaInverse_local = MyMath::inverse(m_data.inertia);
		m_pShape = pBox;
		return pBox;
	}
	Shape* Body::setAsCapsule(const Vector3f& base_, const Vector3f& tip_, float height_, float radius_)
	{
		Shape* pCapsule = new CapsuleShape(base_, tip_, height_, radius_);
		m_pShape = pCapsule;

		return pCapsule;
	}
	Shape* Body::setAsPlane(const Matrix4f& transform_)
	{
		/*Vector4f planePositions[4] = {
				Vector4f(-0.5f, 0.0f,  0.5f, 1.0f),
				Vector4f(-0.5f, 0.0f, -0.5f, 1.0f),
				Vector4f(0.5f, 0.0f,  0.5f, 1.0f),
				Vector4f(0.5f, 0.0f, -0.5f, 1.0f),
		};

		for (size_t i = 0; i < 4; i++)
			planePositions[i] = transform_ * planePositions[i];

		drawLine(lineVertices[0], lineVertices[1], color_);
		drawLine(lineVertices[2], lineVertices[0], color_);

		drawLine(lineVertices[1], lineVertices[2], color_);

		drawLine(lineVertices[1], lineVertices[3], color_);
		drawLine(lineVertices[3], lineVertices[2], color_);

		Shape* pPlane = new TriangleShape(center_, radius_);*/


		return nullptr;
	}

	void Body::addRotationalImpulse(const Vector3f& point_, const Vector3f& impulse_, float dt_)
	{
		Vector3f centerOfMass = m_transform.position;
		Vector3f torque = MyMath::cross(point_ - centerOfMass, impulse_);

		Vector3f angularAcceleration = m_data.inertiaInverse * torque;
		m_data.angularVelocity += angularAcceleration * dt_;
	}
}