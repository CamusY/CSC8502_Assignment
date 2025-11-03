#pragma once

#include "IAL/I_InputDevice.h"
#include "nclgl/Keyboard.h"
#include "nclgl/Mouse.h"

/**
 * @brief Why this adapter uses polling instead of callbacks.
 * @details
 * This adapter uses a simple polling model (checking input every frame)
 * rather than an event-driven callback system like the one suggested in nclgl’s Keyboard.h.
 * Polling keeps the input logic predictable and easy to reason about
 * inside the main game loop — no async events, no hidden state.
 * It also means my application layer doesn’t need to know or care what backend
 * (like nclgl, GLFW, or even something I might write myself)
 * actually provides the input, which makes swapping libraries much easier later on.
 * The small per-frame cost is a fair trade for the clarity, control, and decoupling it provides.
 */

// 在 .h 中使用前向声明
class Keyboard;
class Mouse;

namespace NCLGL_Impl {

    /**
     * @brief 轨道 B (nclgl) 键盘实现
     * @details
     * - 实现了 Engine::IAL::I_Keyboard 接口。
     * - NFR-11.4: 包装由 nclgl::Window 自动创建的静态 Keyboard 实例。
     * - 核心职责：将 IAL::KeyCode 映射到 nclgl::KeyboardKeys。
     */
    class B_Keyboard : public Engine::IAL::I_Keyboard {
    public:
        /**
         * @brief 构造函数
         * @param nclKeyboard 指向 nclgl::Window::GetKeyboard() 返回的静态实例。
         */
        B_Keyboard(Keyboard* nclKeyboard);
        virtual ~B_Keyboard() = default;

        // --- I_Keyboard 接口实现 ---
        virtual bool KeyDown(Engine::IAL::KeyCode key) override;
        virtual bool KeyHeld(Engine::IAL::KeyCode key) override;
        virtual bool KeyTriggered(Engine::IAL::KeyCode key) override;

    private:
        /**
         * @brief 内部转换函数，将 IAL 键码转换为 nclgl 键码。
         */
        KeyboardKeys ConvertKey(Engine::IAL::KeyCode key) const;
        
        Keyboard* m_nclKeyboard; // 指向 nclgl 实例的非拥有指针
    };

    // --- 鼠标适配器 ---

    /**
     * @brief 轨道 B (nclgl) 鼠标实现
     * @details
     * - 实现了 Engine::IAL::I_Mouse 接口。
     * - NFR-11.4: 包装由 nclgl::Window 自动创建的静态 Mouse 实例。
     * - 核心职责：将 IAL::MouseButton 映射到 nclgl::MouseButtons。
     * - IAL 规范: 实现了 nclgl::Mouse 中缺失的 ButtonTriggered() 逻辑。
     */
    class B_Mouse : public Engine::IAL::I_Mouse {
    public:
        /**
         * @brief 构造函数
         * @param nclMouse 指向 nclgl::Window::GetMouse() 返回的静态实例。
         */
        B_Mouse(Mouse* nclMouse);
        virtual ~B_Mouse() = default;

        // --- I_Mouse 接口实现 ---
        virtual Vector2 GetRelativePosition() override;
        virtual Vector2 GetAbsolutePosition() override;

        virtual bool ButtonDown(Engine::IAL::MouseButton button) override;
        virtual bool ButtonHeld(Engine::IAL::MouseButton button) override;
        virtual bool ButtonTriggered(Engine::IAL::MouseButton button) override;
        virtual bool DoubleClicked(Engine::IAL::MouseButton button) override;

        virtual bool WheelMoved() override;
        virtual int GetWheelMovement() override;

    private:
        /**
         * @brief 内部转换函数，将 IAL 按钮码转换为 nclgl 按钮码。
         */
        MouseButtons ConvertButton(Engine::IAL::MouseButton button) const;
        
        Mouse* m_nclMouse; // 指向 nclgl 实例的非拥有指针
    };

} // namespace NCLGL_Impl