/**
 * @file B_InputDevice.h
 * @brief (Day 3) 轨道 B (nclgl) 对 I_InputDevice 接口的适配器实现。
 * @details
 * 本文件提供了 `B_Keyboard` 和 `B_Mouse` 两个类，它们是
 * `nclgl` 库中 `::Keyboard` 和 `::Mouse` 类的 "适配器" 或 "包装器"。
 *
 * (NFR-11.4)
 * 这两个适配器类被设计为轻量级的包装器，它们不拥有底层的 `nclgl`
 * 对象实例。相反，它们在构造时接收一个指向由 `nclgl::Window`
 * 自动创建和管理的 `static` 全局实例的指针。
 *
 * @brief Why this adapter stays with polling instead of per-key callbacks.
 * @details
 * I noticed the suggestion in nclgl's Keyboard.h about adding callbacks for triggered / held keys.
 * I'm consciously not doing that here.
 * For this project, input is processed in a predictable per-frame update loop.
 * Keeping it as polling means all input-related logic stays in one place,
 * instead of being pushed in from event callbacks at arbitrary timing.
 * It's just easier to reason about, debug, and later swap out the backend if needed.
 * So yes, I saw the callback challenge — I just went with the explicit polling flow
 * because it fits the engine structure I'm building right now.
 *
 * @class NCLGL_Impl::B_Keyboard
 * @brief `nclgl::Keyboard` 的轨道 B 适配器。
 * @details
 * 实现了 `Engine::IAL::I_Keyboard` 接口。
 * (NFR-11.4) 构造函数必须接收由 `B_WindowSystem` 提供的、
 * 指向 `nclgl::Window::keyboard` (static 实例) 的指针。
 * 它负责将平台无关的 `IAL::KeyCode` 映射到 `nclgl::KeyboardKeys`。
 *
 * @fn NCLGL_Impl::B_Keyboard::B_Keyboard(::Keyboard* nclKeyboard)
 * @brief 构造函数。
 * @param nclKeyboard 指向 nclgl 库的全局键盘实例的指针。
 *
 * @fn NCLGL_Impl::B_Keyboard::KeyDown
 * @brief (已实现) 委托给 `nclgl::Keyboard::KeyDown`。
 *
 * @fn NCLGL_Impl::B_Keyboard::KeyHeld
 * @brief (已实现) 委托给 `nclgl::Keyboard::KeyHeld`。
 *
 * @fn NCLGL_Impl::B_Keyboard::KeyTriggered
 * @brief (已实现) 委托给 `nclgl::Keyboard::KeyTriggered`。
 *
 * @var NCLGL_Impl::B_Keyboard::m_nclKeyboard
 * @brief 持有的指向 nclgl 键盘实例的非拥有指针。
 *
 * @class NCLGL_Impl::B_Mouse
 * @brief `nclgl::Mouse` 的轨道 B 适配器。
 * @details
 * 实现了 `Engine::IAL::I_Mouse` 接口。
 * (NFR-11.4) 构造函数必须接收由 `B_WindowSystem` 提供的、
 * 指向 `nclgl::Window::mouse` (static 实例) 的指针。
 * 它负责将平台无关的 `IAL::MouseButton` 映射到 `nclgl::MouseButtons`。
 *
 * @fn NCLGL_Impl::B_Mouse::B_Mouse(::Mouse* nclMouse)
 * @brief 构造函数。
 * @param nclMouse 指向 nclgl 库的全局鼠标实例的指针。
 *
 * @fn NCLGL_Impl::B_Mouse::GetRelativePosition
 * @brief (已实现) 委托给 `nclgl::Mouse::GetRelativePosition`。
 *
 * @fn NCLGL_Impl::B_Mouse::GetAbsolutePosition
 * @brief (已实现) 委托给 `nclgl::Mouse::GetAbsolutePosition`。
 *
 * @fn NCLGL_Impl::B_Mouse::ButtonDown
 * @brief (已实现) 委托给 `nclgl::Mouse::ButtonDown`。
 *
 * @fn NCLGL_Impl::B_Mouse::ButtonHeld
 * @brief (已实现) 委托给 `nclgl::Mouse::ButtonHeld`。
 *
 * @fn NCLGL_Impl::B_Mouse::ButtonTriggered
 * @brief (已实现) 通过 `ButtonDown`/`ButtonHeld` 组合判断触发。
 *
 * @fn NCLGL_Impl::B_Mouse::ButtonDoubleClicked
 * @brief (已实现) 委托给 `nclgl::Mouse::DoubleClicked`。
 *
 * @fn NCLGL_Impl::B_Mouse::WheelMoved
 * @brief (已实现) 委托给 `nclgl::Mouse::WheelMoved`。
 *
 * @fn NCLGL_Impl::B_Mouse::GetWheelMovement
 * @brief (已实现) 委托给 `nclgl::Mouse::GetWheelMovement`。
 *
 * @var NCLGL_Impl::B_Mouse::m_nclMouse
 * @brief 持有的指向 nclgl 鼠标实例的非拥有指针。
 */

#pragma once
#include "IAL/I_InputDevice.h"

// (NFR-1) B_ 轨实现被授权包含 nclgl 具体实现
#include "nclgl/Keyboard.h"
#include "nclgl/Mouse.h"

namespace NCLGL_Impl {

    class B_Keyboard : public Engine::IAL::I_Keyboard {
    public:
        // (NFR-11.4) 构造函数必须接收由 nclgl::Window 创建的 static 实例
        B_Keyboard(::Keyboard* nclKeyboard) : m_nclKeyboard(nclKeyboard) {}
        ~B_Keyboard() {}

        bool KeyDown(Engine::IAL::KeyCode key) override;
        bool KeyHeld(Engine::IAL::KeyCode key) override;
        bool KeyTriggered(Engine::IAL::KeyCode key) override;

    private:
        ::Keyboard* m_nclKeyboard; // (NFR-11.4) 包装 nclgl 实例
    };

    class B_Mouse : public Engine::IAL::I_Mouse {
    public:
        // (NFR-11.4) 构造函数必须接收由 nclgl::Window 创建的 static 实例
        B_Mouse(::Mouse* nclMouse) : m_nclMouse(nclMouse) {}
        ~B_Mouse() {}

        ::Vector2 GetRelativePosition() override;
        ::Vector2 GetAbsolutePosition() override;

        bool ButtonDown(Engine::IAL::MouseButton button) override;
        bool ButtonHeld(Engine::IAL::MouseButton button) override;
        bool ButtonTriggered(Engine::IAL::MouseButton button) override;
        bool ButtonDoubleClicked(Engine::IAL::MouseButton button) override;

        bool WheelMoved() override;
        int GetWheelMovement() override;

    private:
        ::Mouse* m_nclMouse; // (NFR-11.4) 包装 nclgl 实例
    };

} // namespace NCLGL_Impl