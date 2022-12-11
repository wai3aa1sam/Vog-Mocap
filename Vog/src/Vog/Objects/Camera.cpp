#include "vogpch.h"
#include "Camera.h"

namespace vog {

	Camera::Camera(const Matrix4f& projectionMatrix_)
		:
		m_projectionMatrix(projectionMatrix_)
	{
		_recalculateViewMatrix();
	}

	Camera::Camera(const PerspectiveCameraParameters& perspectiveCameraParameters_)
	{
		m_projectionMatrix = MyMath::perspective(perspectiveCameraParameters_.fovy, perspectiveCameraParameters_.aspect,
			perspectiveCameraParameters_.nearPlane, perspectiveCameraParameters_.farPlane);
		_recalculateViewMatrix();
	}

	Camera::Camera(const OrthographicCameraParameters& orthographicCameraParameters_)
	{
		m_projectionMatrix = MyMath::orthographic(orthographicCameraParameters_.left, orthographicCameraParameters_.right,
			orthographicCameraParameters_.bottom, orthographicCameraParameters_.top);
		_recalculateViewMatrix();
	}

	Camera::~Camera()
	{
	}

	void Camera::setProjection(const Matrix4f& projectionMatrix_)
	{
		m_projectionMatrix = projectionMatrix_;
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}

	void Camera::setProjection(const PerspectiveCameraParameters& perspectiveCameraParameters_)
	{
		m_projectionMatrix = MyMath::perspective(perspectiveCameraParameters_.fovy, perspectiveCameraParameters_.aspect,
			perspectiveCameraParameters_.nearPlane, perspectiveCameraParameters_.farPlane);
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}

	void Camera::setProjection(const OrthographicCameraParameters& orthographicCameraParameters_)
	{
		m_projectionMatrix = MyMath::orthographic(orthographicCameraParameters_.left, orthographicCameraParameters_.right,
			orthographicCameraParameters_.bottom, orthographicCameraParameters_.top);
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}

	Matrix4f Camera::getViewRotationMatrix() const
	{
		Matrix4f viewRotationMatrix = getViewMatrix();
		viewRotationMatrix[3][0] = 0;
		viewRotationMatrix[3][1] = 0;
		viewRotationMatrix[3][2] = 0;
		return viewRotationMatrix;
	}

	void Camera::_recalculateViewMatrix()
	{
		m_viewMatrix = MyMath::lookAt(m_cameraBaiscParams.position, m_cameraBaiscParams.position + m_cameraBaiscParams.front, m_cameraBaiscParams.up);
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
		m_inverseViewProjectionMatrix = MyMath::inverse(m_viewProjectionMatrix);
	}
}