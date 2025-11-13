/**
 * @file Camera.cpp
 * @brief 实现轨迹与自由双模式相机的输入驱动与视图矩阵逻辑。
 */
#include "Camera.h"

#include "nclgl/Vector2.h"

#include <algorithm>
#include <cmath>

namespace {
    constexpr float kRadToDeg = 57.29577951308232f;

    Vector3 CatmullRom(const Vector3& p0,
                       const Vector3& p1,
                       const Vector3& p2,
                       const Vector3& p3,
                       float t) {
        const float t2 = t * t;
        const float t3 = t2 * t;

        Vector3 term1 = p1 * 2.0f;

        Vector3 term2 = (p2 - p0) * t;

        Vector3 term3 = (p0 * 2.0f - p1 * 5.0f + p2 * 4.0f - p3) * t2;

        Vector3 term4 = (-p0 + p1 * 3.0f - p2 * 3.0f + p3) * t3;

        Vector3 result = term1 + term2 + term3 + term4;

        return result * 0.5f;
    }


    Vector3 CatmullRomTangent(const Vector3& p0,
                              const Vector3& p1,
                              const Vector3& p2,
                              const Vector3& p3,
                              float t) {
        const float t2 = t * t;

        Vector3 term1 = (p2 - p0);

        Vector3 term2 = (p0 * 4.0f - p1 * 10.0f + p2 * 8.0f - p3 * 2.0f) * t;

        Vector3 term3 = (-p0 * 3.0f + p1 * 9.0f - p2 * 9.0f + p3 * 3.0f) * t2;

        Vector3 result = term1 + term2 + term3;

        return result * 0.5f;
    }
}

Camera::Camera() :
    m_position(Vector3(0.0f, 180.0f, 50.0f))
    , m_yaw(-150.0f)
    , m_pitch(-35.0f)
    , m_moveSpeed(50.0f)
    , m_mouseSensitivity(1.1f)
    , m_mode(Mode::Track)
    , m_trackPoints()
    , m_trackProgress(0.0f)
    , m_trackSpeed(0.18f) {
    m_trackPoints = {
        Vector3(420.0f, 38.0f, 300.0f),
        Vector3(560.0f, 40.0f, 520.0f),
        Vector3(700.0f, 42.0f, 740.0f),
        Vector3(520.0f, 38.0f, 900.0f),
        Vector3(380.0f, 36.0f, 760.0f),
        Vector3(340.0f, 35.0f, 520.0f)
    };

    ResetTrack();
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
    if (m_mode == Mode::Track) {
        UpdateTrack(deltaTime);
        return;
    }

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
        if (keyboard->KeyHeld(Engine::IAL::KeyCode::LEFT_SHIFT)
            || keyboard->KeyHeld(Engine::IAL::KeyCode::RIGHT_SHIFT)) {
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

void Camera::SetMode(Mode mode) {
    if (m_mode == mode) {
        return;
    }
    m_mode = mode;
    if (m_mode == Mode::Track) {
        ResetTrack();
    }
}

Camera::Mode Camera::GetMode() const {
    return m_mode;
}

void Camera::SetTrackPoints(const std::vector<Vector3>& points) {
    m_trackPoints = points;
    if (m_mode == Mode::Track) {
        ResetTrack();
    }
    else {
        m_trackProgress = 0.0f;
    }
}

void Camera::ResetTrack() {
    m_trackProgress = 0.0f;
    if (!HasValidTrack()) {
        return;
    }
    m_position = m_trackPoints.front();
    Vector3 tangent = EvaluateTrackTangent(0, 0.0f);
    if (tangent.Length() > 0.0f) {
        tangent.Normalise();
        m_yaw = std::atan2(tangent.x, -tangent.z) * kRadToDeg;
        const float clampedY = std::clamp(tangent.y, -1.0f, 1.0f);
        m_pitch = std::asin(clampedY) * kRadToDeg;
        m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    }
}

void Camera::SetTrackSpeed(float speed) {
    m_trackSpeed = std::max(speed, 0.0f);
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

void Camera::UpdateTrack(float deltaTime) {
    if (!HasValidTrack()) {
        return;
    }
    const float segmentCount = static_cast<float>(m_trackPoints.size());
    m_trackProgress += m_trackSpeed * deltaTime;
    while (m_trackProgress >= segmentCount) {
        m_trackProgress -= segmentCount;
    }
    while (m_trackProgress < 0.0f) {
        m_trackProgress += segmentCount;
    }

    const int segmentIndex = static_cast<int>(m_trackProgress);
    const float segmentT = m_trackProgress - static_cast<float>(segmentIndex);

    Vector3 position = EvaluateTrackPosition(segmentIndex, segmentT);
    Vector3 tangent = EvaluateTrackTangent(segmentIndex, segmentT);

    m_position = position;
    if (tangent.Length() > 0.0f) {
        tangent.Normalise();
        m_yaw = std::atan2(tangent.x, -tangent.z) * kRadToDeg;
        const float clampedY = std::clamp(tangent.y, -1.0f, 1.0f);
        m_pitch = std::asin(clampedY) * kRadToDeg;
        m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    }
}

Vector3 Camera::EvaluateTrackPosition(int segmentIndex, float t) const {
    const Vector3& p0 = m_trackPoints[WrapTrackIndex(segmentIndex - 1)];
    const Vector3& p1 = m_trackPoints[WrapTrackIndex(segmentIndex)];
    const Vector3& p2 = m_trackPoints[WrapTrackIndex(segmentIndex + 1)];
    const Vector3& p3 = m_trackPoints[WrapTrackIndex(segmentIndex + 2)];
    return CatmullRom(p0, p1, p2, p3, std::clamp(t, 0.0f, 1.0f));
}

Vector3 Camera::EvaluateTrackTangent(int segmentIndex, float t) const {
    const Vector3& p0 = m_trackPoints[WrapTrackIndex(segmentIndex - 1)];
    const Vector3& p1 = m_trackPoints[WrapTrackIndex(segmentIndex)];
    const Vector3& p2 = m_trackPoints[WrapTrackIndex(segmentIndex + 1)];
    const Vector3& p3 = m_trackPoints[WrapTrackIndex(segmentIndex + 2)];
    return CatmullRomTangent(p0, p1, p2, p3, std::clamp(t, 0.0f, 1.0f));
}

int Camera::WrapTrackIndex(int index) const {
    if (!HasValidTrack()) {
        return 0;
    }
    const int count = static_cast<int>(m_trackPoints.size());
    int wrapped = index % count;
    if (wrapped < 0) {
        wrapped += count;
    }
    return wrapped;
}

bool Camera::HasValidTrack() const {
    return m_trackPoints.size() >= 4;
}
