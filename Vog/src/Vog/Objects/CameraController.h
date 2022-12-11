#pragma once
#include "Vog/Core/Core.h"
#include "Vog/Events/Event.h"
#include "Vog/Events/ApplicationEvent.h"
#include "Vog/Events/MouseEvent.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Camera.h"

namespace vog {

	class VOG_API CameraController : public NonCopyable
	{
	public:
		CameraController(const Matrix4f& projectionMatrix_);
		CameraController(const PerspectiveCameraParameters& perspectiveCameraParameters_ = PerspectiveCameraParameters{});
		CameraController(const OrthographicCameraParameters& orthographicCameraParameters_);
		~CameraController();

		void onUpdate(float dt_);
		void onEvent(Event& event_);

		inline float getAspect() const { return m_aspect; }
		void setAspect(float aspect_);

		inline Camera& getCamera() { return m_camera; }
		inline const Camera& getCamera() const { return m_camera; }

		inline float getSpeed() const { return m_moveSpeed; }
		inline float& getSpeed() { return m_moveSpeed; }
		
		inline float getRotationtSpeed() const { return m_rotationSpeed; }
		inline float& getRotationtSpeed() { return m_rotationSpeed; }

	private:
		void _processInput(float dt_, const std::pair<float, float>& mousePosition_);
		void _updateRotation(float dx_, float dy_);
		void _updateCameraVector();

		bool _onMouseScrolled(MouseScrolledEvent& event_);
	private:
		Camera m_camera;
		Vector3f m_position;
		Vector3f m_right;
		float m_aspect = 1.7778f;
		float m_fovy = MyMath::radians(45.0f);

		float m_pitch = 0.0f, m_yaw = -90.0f;
		float m_moveSpeed = 15.0f;
		float m_rotationSpeed = 90.0f;

		std::pair<float, float> m_lastMousePosition = {0.0f, 0.0f};
	};
}