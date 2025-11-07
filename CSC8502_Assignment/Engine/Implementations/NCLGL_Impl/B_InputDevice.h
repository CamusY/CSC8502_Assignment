/**
 * @file B_InputDevice.h
 * @brief 轨道 B (NCLGL_Impl) 的输入设备接口实现。
 *
 * 本文件定义了 B_Keyboard 和 B_Mouse 类，它们分别是 Engine::IAL::I_Keyboard 和 
 * Engine::IAL::I_Mouse 接口在 nclgl 框架下的具体适配器实现。
 *
 * B_Keyboard 类 (NCLGL_Impl::B_Keyboard):
 * 继承自 Engine::IAL::I_Keyboard 纯虚接口。
 * 负责适配 nclgl::Keyboard。在 Day 2 阶段，它暂时不持有任何实际对象，仅提供空接口。
 *
 * B_Keyboard::KeyDown(Engine::IAL::KeyCode key):
 * 检查指定按键是否被按下。Day 2 返回 false。
 *
 * B_Keyboard::KeyHeld(Engine::IAL::KeyCode key):
 * 检查指定按键是否被持续按住。Day 2 返回 false。
 *
 * B_Keyboard::KeyTriggered(Engine::IAL::KeyCode key):
 * 检查指定按键是否在本帧被触发。Day 2 返回 false。
 *
 * B_Mouse 类 (NCLGL_Impl::B_Mouse):
 * 继承自 Engine::IAL::I_Mouse 纯虚接口。
 * 负责适配 nclgl::Mouse。
 *
 * B_Mouse::GetRelativePosition():
 * 获取鼠标自上一帧以来的相对移动量。Day 2 返回零向量。
 *
 * B_Mouse::ButtonDown(Engine::IAL::MouseButton button):
 * 检查指定鼠标按键是否被按下。Day 2 返回 false。
 *
 * B_Mouse::ButtonHeld(Engine::IAL::MouseButton button):
 * 检查指定鼠标按键是否被持续按住。Day 2 返回 false。
 *
 * B_Mouse::ButtonTriggered(Engine::IAL::MouseButton button):
 * 检查指定鼠标按键是否在本帧被触发。Day 2 返回 false。
 *
 * B_Mouse::GetAbsolutePosition():
 * 获取鼠标在窗口中的绝对位置。Day 2 返回零向量。
 */
#pragma once
#include "IAL/I_InputDevice.h"

namespace NCLGL_Impl {

    class B_Keyboard : public Engine::IAL::I_Keyboard {
    public:
        B_Keyboard();
        ~B_Keyboard() override;

        bool KeyDown(Engine::IAL::KeyCode key) override;
        bool KeyHeld(Engine::IAL::KeyCode key) override;
        bool KeyTriggered(Engine::IAL::KeyCode key) override;
    };

    class B_Mouse : public Engine::IAL::I_Mouse {
    public:
        B_Mouse();
        ~B_Mouse() override;

        Vector2 GetRelativePosition() override;
        Vector2 GetAbsolutePosition() override;
        bool ButtonDown(Engine::IAL::MouseButton button) override;
        bool ButtonHeld(Engine::IAL::MouseButton button) override;
        bool ButtonTriggered(Engine::IAL::MouseButton button) override;
    };

}