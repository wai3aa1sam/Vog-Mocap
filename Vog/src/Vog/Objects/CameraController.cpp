#include "vogpch.h"
#include "CameraController.h"

#include "Vog/Core/Input.h"
#include "Vog/Core/Timer.h"
#include "Vog/Core/Keycodes.h"

namespace vog {
    CameraController::CameraController(const Matrix4f& projectionMatrix_)
        :
        m_camera(projectionMatrix_), m_position(m_camera.getPosition()), m_right(MyMath::cross(m_camera.getFront(), m_camera.getCameraBasicParameters().up))
    {
    }

    CameraController::CameraController(const PerspectiveCameraParameters& perspectiveCameraParameters_)
        :
        m_camera(perspectiveCameraParameters_), m_position(m_camera.getPosition()), m_right(MyMath::cross(m_camera.getFront(), m_camera.getCameraBasicParameters().up))
    {

    }

    CameraController::CameraController(const OrthographicCameraParameters& orthographicCameraParameters_)
        :
        m_camera(orthographicCameraParameters_), m_position(m_camera.getPosition()), m_right(MyMath::cross(m_camera.getFront(), m_camera.getCameraBasicParameters().up))
    {
    }

    CameraController::~CameraController()
    {
    }

    void CameraController::onUpdate(float dt_)
    {
        //float dt = static_cast<float>(Timer::getTime());
        float dt = dt_;
        auto mousePosition = Input::getMousePosition();
        //VOG_LOG_TRACE(dt);

        _processInput(dt, mousePosition);

        m_lastMousePosition = mousePosition;
    }

    void CameraController::onEvent(Event& event_)
    {
        EventDispatcher dispatcher(event_);
        dispatcher.dispatch<MouseScrolledEvent>(BIND_EVENT_CALLBACK(CameraController::_onMouseScrolled));
    }

    void CameraController::setAspect(float aspect_)
    {
        m_aspect = aspect_;
        PerspectiveCameraParameters params;
        params.aspect = aspect_;
        m_camera.setProjection(params);
    }

    void CameraController::_processInput(float dt_, const std::pair<float, float>& mousePosition_)
    {
        if (Input::isKeyPressed(KeyCode::W))
            m_position += m_camera.getCameraBasicParameters().front * m_moveSpeed * dt_;
        else if (Input::isKeyPressed(KeyCode::S))
            m_position -= m_camera.getCameraBasicParameters().front * m_moveSpeed * dt_;

        if (Input::isKeyPressed(KeyCode::D))
            m_position += m_right * m_moveSpeed * dt_;
        else if (Input::isKeyPressed(KeyCode::A))
            m_position -= m_right * m_moveSpeed * dt_;

        if (Input::isMousePressed(KeyCode::RButton))
        {
            float mouseDx = mousePosition_.first - m_lastMousePosition.first;
            float mouseDy = mousePosition_.second - m_lastMousePosition.second;
            mouseDx *= m_rotationSpeed * dt_;
            mouseDy *= m_rotationSpeed * dt_;
            _updateRotation(mouseDx, -mouseDy);
        }
        else
        {
            m_camera.setPosition(m_position);
        }
    }

    void CameraController::_updateRotation(float dx_, float dy_)
    {
        m_yaw += dx_;
        m_pitch += dy_;

        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;

        _updateCameraVector();
    }

    void CameraController::_updateCameraVector()
    {
        CameraBasicParameters params;

        Vector3f front;
        front.x = cos(MyMath::radians(m_yaw)) * cos(MyMath::radians(m_pitch));
        front.y = sin(MyMath::radians(m_pitch));
        front.z = sin(MyMath::radians(m_yaw)) * cos(MyMath::radians(m_pitch));
        front = MyMath::normalize(front);
        m_right = MyMath::normalize(MyMath::cross(front, MyMath::up()));

        params.position = m_position;
        params.front = front;
        params.up = MyMath::cross(m_right, front);

        params.pitch = m_pitch;
        params.yaw = m_yaw;

        m_camera.setCameraBasicParameters(params);
    }

    bool CameraController::_onMouseScrolled(MouseScrolledEvent& event_)
    {
        PerspectiveCameraParameters params;
        m_fovy += event_.getOffsetY();
        m_fovy = std::min(std::max(m_fovy, 0.25f), 2.0f);
        params.fovy = m_fovy;
        params.aspect = m_aspect;
        m_camera.setProjection(params);
        return false;
    }
}