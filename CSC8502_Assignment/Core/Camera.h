/**
* @file Camera.h
 * @brief 声明自由相机控制与视图矩阵生成的核心类。
 *
 * 本文件定义的 Camera 类负责维护相机在世界空间中的位置与欧拉角姿态，
 * 并基于 IAL 输入接口驱动自由飞行相机的更新逻辑。相机更新会根据键盘与
 * 鼠标输入计算位移与旋转，同时提供 BuildViewMatrix 接口以调用 nclgl::Matrix4::BuildViewMatrix，
 * 供渲染器生成视图矩阵使用。
 */
#pragma once

#include "nclgl/Vector3.h"
#include "nclgl/Matrix4.h"
#include "../Engine/IAL/I_InputDevice.h"

class Camera {
public:
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

private:
    Vector3 CalculateForward() const;
    Vector3 CalculateRight() const;

    Vector3 m_position;
    float m_yaw;
    float m_pitch;
    float m_moveSpeed;
    float m_mouseSensitivity;
};