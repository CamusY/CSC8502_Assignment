/**
 * @file Camera.cpp
 * @brief 实现自由相机的输入驱动与视图矩阵构建逻辑。
 */
#include "Camera.h"

#include "nclgl/Vector2.h"

#include <algorithm>

Camera::Camera()
    : m_position(Vector3(0.0f, 150.0f, 3.5f))
    , m_yaw(-150.0f)
    , m_pitch(-50.0f)
    , m_moveSpeed(50.0f)
    , m_mouseSensitivity(1.1f) {
}

void Camera::SetPosition(const Vector3& position) {
    m_position = position;
}

const Vector3& Camera::GetPosition() const {
    return m_position;
}

void Camera::SetYaw(float yawDegrees) {
    m_yaw = yawDegrees;
}

float Camera::GetYaw() const {
    return m_yaw;
}

void Camera::SetPitch(float pitchDegrees) {
    m_pitch = pitchDegrees;
}

float Camera::GetPitch() const {
    return m_pitch;
}

void Camera::Update(float deltaTime,
                    Engine::IAL::I_Keyboard* keyboard,
                    Engine::IAL::I_Mouse* mouse) {
    if (!keyboard) {
        return;
    }

    Vector3 forward = CalculateForward();
    Vector3 right = CalculateRight();
    Vector3 up(0.0f, 1.0f, 0.0f);
    Vector3 movement;

    if (keyboard->KeyHeld(Engine::IAL::KeyCode::W)) {
        movement += forward;
    }
    if (keyboard->KeyHeld(Engine::IAL::KeyCode::S)) {
        movement -= forward;
    }
    if (keyboard->KeyHeld(Engine::IAL::KeyCode::D)) {
        movement += right;
    }
    if (keyboard->KeyHeld(Engine::IAL::KeyCode::A)) {
        movement -= right;
    }
    if (keyboard->KeyHeld(Engine::IAL::KeyCode::SPACE)) {
        movement += up;
    }
    if (keyboard->KeyHeld(Engine::IAL::KeyCode::LEFT_CTRL)) {
        movement -= up;
    }

    if (movement.Length() > 0.0f) {
        movement.Normalise();

        float currentSpeed = m_moveSpeed;
        if (keyboard->KeyHeld(Engine::IAL::KeyCode::LEFT_SHIFT) ||
            keyboard->KeyHeld(Engine::IAL::KeyCode::RIGHT_SHIFT)) {
            currentSpeed *= 10.0f;
            }

        m_position += movement * (currentSpeed * deltaTime);
    }

    if (!mouse) {
        return;
    }

    Vector2 mouseDelta = mouse->GetRelativePosition();
    m_yaw -= mouseDelta.x * m_mouseSensitivity;
    m_pitch -= mouseDelta.y * m_mouseSensitivity;
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
}

Matrix4 Camera::BuildViewMatrix() const {
    Vector3 forward = CalculateForward();
    Vector3 target = m_position + forward;
    return Matrix4::BuildViewMatrix(m_position, target);
}

Vector3 Camera::CalculateForward() const {
    Matrix4 yawMatrix = Matrix4::Rotation(m_yaw, Vector3(0.0f, 1.0f, 0.0f));
    Matrix4 pitchMatrix = Matrix4::Rotation(m_pitch, Vector3(1.0f, 0.0f, 0.0f));
    Matrix4 orientation = yawMatrix * pitchMatrix;
    Vector3 forward = orientation * Vector3(0.0f, 0.0f, -1.0f);
    forward.Normalise();
    return forward;
}

Vector3 Camera::CalculateRight() const {
    Vector3 forward = CalculateForward();
    Vector3 right = Vector3::Cross(forward, Vector3(0.0f, 1.0f, 0.0f));
    right.Normalise();
    return right;
}