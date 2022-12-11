#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

namespace vog {

#pragma region Camera_Parameters
	struct VOG_API CameraBasicParameters
	{
		CameraBasicParameters(const Vector3f& position_ = { 0.0f, 0.0f, 3.0f }, const Vector3f& front_ = { 0.0f, 0.0f, -1.0f }, const Vector3f& up_ = { 0.0f, 1.0f, 0.0f })
			:
			position(position_), front(front_), up(up_)
		{}
		Vector3f position = { 0.0f, 0.0f, 3.0f };
		Vector3f front = { 0.0f, 0.0f, -1.0f };
		Vector3f up = { 0.0f, 1.0f, 0.0f };
		float pitch = 0.0f, yaw = 0.0f;
	};

	struct VOG_API PerspectiveCameraParameters
	{
		//PerspectiveCameraParameters(float fovy_rad_ = MyMath::radians(45.0f), float aspect_ = 1.778f, float zNear_ = 0.01f, float zFar_ = 10000.0f)
		//	:
		//	fovy(fovy_rad_), aspect(aspect_), nearPlane(zNear_), farPlane(zFar_)
		//{}
		float fovy = MyMath::radians(45.0f), aspect = 1.778f, nearPlane = 0.01f, farPlane = 10000.0f;
	};

	struct VOG_API OrthographicCameraParameters
	{
		OrthographicCameraParameters(float left_, float right_, float bottom_, float top_)
			:
			left(left_), right(right_), bottom(bottom_), top(top_)
		{}
		float left, right, bottom, top;
	};
#pragma endregion

	class VOG_API Camera : public NonCopyable
	{
	public:
		Camera(const Matrix4f& projectionMatrix_);
		Camera(const PerspectiveCameraParameters& perspectiveCameraParameters_);
		Camera(const OrthographicCameraParameters& orthographicCameraParameters_);
		//Camera(float fovy_rad_, float aspect_, float zNear_, float zFar_);
		//Camera(float left_, float right_, float bottom_, float top_);

		~Camera();

		void setProjection(const Matrix4f& projectionMatrix_);
		void setProjection(const PerspectiveCameraParameters& perspectiveCameraParameters_);
		void setProjection(const OrthographicCameraParameters& orthographicCameraParameters_);

		inline const CameraBasicParameters& getCameraBasicParameters() const { return m_cameraBaiscParams; }
		void setCameraBasicParameters(const CameraBasicParameters& cameraBasicParameters_) { m_cameraBaiscParams = cameraBasicParameters_; _recalculateViewMatrix(); }

		inline Quaternion getOrientation() const { return Quaternion(Vector3f(-m_cameraBaiscParams.pitch, -m_cameraBaiscParams.yaw, 0.0f)); }

		inline const Vector3f& getPosition() const { return m_cameraBaiscParams.position; }
		void setPosition(const glm::vec3& position_) { m_cameraBaiscParams.position = position_; _recalculateViewMatrix(); }

		inline const Vector3f& getFront() const { return m_cameraBaiscParams.front; }
		void setFront(const glm::vec3& front_) { m_cameraBaiscParams.front = front_; _recalculateViewMatrix(); }

		inline const Matrix4f& getProjectionMatrix() const { return m_projectionMatrix; }
		inline const Matrix4f& getViewMatrix() const { return m_viewMatrix; }
		inline const Matrix4f& getViewProjectionMatrix() const { return m_viewProjectionMatrix; }
		inline const Matrix4f& getInverseViewProjectionMatrix() const { return m_inverseViewProjectionMatrix; }

		Matrix4f getViewRotationMatrix() const;


		//inline Matrix4f getViewMatrix() const { return m_viewMatrix; }


	private:
		void _recalculateViewMatrix();
	private:
		Matrix4f m_projectionMatrix;
		Matrix4f m_viewMatrix;
		Matrix4f m_viewProjectionMatrix;
		Matrix4f m_inverseViewProjectionMatrix;

		CameraBasicParameters m_cameraBaiscParams;
	};
}