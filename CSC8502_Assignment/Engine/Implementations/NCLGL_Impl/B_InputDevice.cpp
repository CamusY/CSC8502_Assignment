/**
* @file B_InputDevice.cpp
 * @brief 轨道 B (NCLGL_Impl) 的输入设备接口实现源文件。
 *
 * 本文件实现了 B_Keyboard 和 B_Mouse 类。
 * 在 Day 2 阶段，所有函数均为返回默认值（false 或 0 向量）的空壳实现。
 * 它们将在后续阶段通过包装 nclgl 的静态输入设备实例来实现具体功能。
 */
#include "B_InputDevice.h"

namespace NCLGL_Impl {

    B_Keyboard::B_Keyboard() {
    }

    B_Keyboard::~B_Keyboard() {
    }

    bool B_Keyboard::KeyDown(Engine::IAL::KeyCode key) {
        return false;
    }

    bool B_Keyboard::KeyHeld(Engine::IAL::KeyCode key) {
        return false;
    }

    bool B_Keyboard::KeyTriggered(Engine::IAL::KeyCode key) {
        return false;
    }

    B_Mouse::B_Mouse() {
    }

    B_Mouse::~B_Mouse() {
    }

    Vector2 B_Mouse::GetRelativePosition() {
        return Vector2(0.0f, 0.0f);
    }

    Vector2 B_Mouse::GetAbsolutePosition() {
        return Vector2(0.0f, 0.0f);
    }

    bool B_Mouse::ButtonDown(Engine::IAL::MouseButton button) {
        return false;
    }

    bool B_Mouse::ButtonHeld(Engine::IAL::MouseButton button) {
        return false;
    }

    bool B_Mouse::ButtonTriggered(Engine::IAL::MouseButton button) {
        return false;
    }

}