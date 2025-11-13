/**
* @file Camera.h
 * @brief 声明同时支持轨迹与自由模式的相机控制核心类。
 *
 * 本文件定义的 Camera 类负责维护相机在世界空间中的位置与欧拉角姿态。
 * Day19 起新增轨迹模式：相机会沿 Catmull-Rom 样条在预设点之间自动飞行；
 * 在自由模式下，相机仍根据 IAL 输入接口响应键鼠操作。BuildViewMatrix
 * 接口继续调用 nclgl::Matrix4::BuildViewMatrix 供渲染器使用。
 */
#pragma once

#include "nclgl/Vector3.h"
#include "nclgl/Matrix4.h"
#include "../Engine/IAL/I_InputDevice.h"

#include <vector>

class Camera {
public:
    enum class Mode {
        Track,
        Free
    };

    Camera();

    void SetPosition(const Vector3& position);
    const Vector3& GetPosition() const;

    void SetYaw(float yawDegrees);
    float GetYaw() const;

    void SetPitch(float pitchDegrees);
    float GetPitch() const;

    void Update(float deltaTime,
                Engine::IAL::I_Keyboard* keyboard,
                Engine::IAL::I_Mouse* mouse);

    Matrix4 BuildViewMatrix() const;

    void SetMode(Mode mode);
    Mode GetMode() const;
    void SetTrackPoints(const std::vector<Vector3>& points);
    void ResetTrack();
    void SetTrackSpeed(float speed);

private:
    Vector3 CalculateForward() const;
    Vector3 CalculateRight() const;
    void UpdateTrack(float deltaTime);
    Vector3 EvaluateTrackPosition(int segmentIndex, float t) const;
    Vector3 EvaluateTrackTangent(int segmentIndex, float t) const;
    int WrapTrackIndex(int index) const;
    bool HasValidTrack() const;

    Vector3 m_position;
    float m_yaw;
    float m_pitch;
    float m_moveSpeed;
    float m_mouseSensitivity;
    Mode m_mode;
    std::vector<Vector3> m_trackPoints;
    float m_trackProgress;
    float m_trackSpeed;
    bool m_firstTrackFrame;
};