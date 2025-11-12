/**
 * @file B_InputDevice.cpp
 * @brief (Day 3) 轨道 B (nclgl) 对 I_InputDevice 接口的实现。
 * @details
 * 实现了 `B_Keyboard` 和 `B_Mouse` 适配器类的所有功能。
 * 核心职责是：
 * 1.  实现 `IAL::KeyCode` 到 `nclgl::KeyboardKeys` 的映射 (MapKey)。
 * 2.  实现 `IAL::MouseButton` 到 `nclgl::MouseButtons` 的映射 (MapButton)。
 * 3.  将所有接口调用委托给被包装的 `nclgl` 实例，并在此过程中
 * 使用 `MapKey` 和 `MapButton` 进行转换。
 * 4.  (NFR-1) 明确包含 nclgl 头文件以访问底层键鼠。
 * 5.  (NFR-11.4) 增加空指针检查，确保在 `nclgl` 实例无效时程序不会崩溃。
 *
 * @fn NCLGL_Impl::MapKey(Engine::IAL::KeyCode ialKey)
 * @brief (Day 3 任务 4)
 * 静态辅助函数，将平台无关的 `IAL::KeyCode` 转换为 `nclgl::KeyboardKeys`。
 * @param ialKey 要转换的 IAL 键码。
 * @return 对应的 nclgl 键码。
 *
 * @fn NCLGL_Impl::MapButton(Engine::IAL::MouseButton ialButton)
 * @brief (Day 3 任务 4)
 * 静态辅助函数，将平台无关的 `IAL::MouseButton` 转换为 `nclgl::MouseButtons`。
 * @param ialButton 要转换的 IAL 鼠标按键。
 * @return 对应的 nclgl 鼠标按键。
 */

#include "B_InputDevice.h"

#include "nclgl/Keyboard.h"
#include "nclgl/Mouse.h"

namespace NCLGL_Impl {
    namespace {

        KeyboardKeys MapKey(Engine::IAL::KeyCode k) {
            switch (k) {
            case Engine::IAL::KeyCode::A:
                return KEYBOARD_A;
            case Engine::IAL::KeyCode::B:
                return KEYBOARD_B;
            case Engine::IAL::KeyCode::C:
                return KEYBOARD_C;
            case Engine::IAL::KeyCode::D:
                return KEYBOARD_D;
            case Engine::IAL::KeyCode::E:
                return KEYBOARD_E;
            case Engine::IAL::KeyCode::F:
                return KEYBOARD_F;
            case Engine::IAL::KeyCode::G:
                return KEYBOARD_G;
            case Engine::IAL::KeyCode::H:
                return KEYBOARD_H;
            case Engine::IAL::KeyCode::I:
                return KEYBOARD_I;
            case Engine::IAL::KeyCode::J:
                return KEYBOARD_J;
            case Engine::IAL::KeyCode::K:
                return KEYBOARD_K;
            case Engine::IAL::KeyCode::L:
                return KEYBOARD_L;
            case Engine::IAL::KeyCode::M:
                return KEYBOARD_M;
            case Engine::IAL::KeyCode::N:
                return KEYBOARD_N;
            case Engine::IAL::KeyCode::O:
                return KEYBOARD_O;
            case Engine::IAL::KeyCode::P:
                return KEYBOARD_P;
            case Engine::IAL::KeyCode::Q:
                return KEYBOARD_Q;
            case Engine::IAL::KeyCode::R:
                return KEYBOARD_R;
            case Engine::IAL::KeyCode::S:
                return KEYBOARD_S;
            case Engine::IAL::KeyCode::T:
                return KEYBOARD_T;
            case Engine::IAL::KeyCode::U:
                return KEYBOARD_U;
            case Engine::IAL::KeyCode::V:
                return KEYBOARD_V;
            case Engine::IAL::KeyCode::W:
                return KEYBOARD_W;
            case Engine::IAL::KeyCode::X:
                return KEYBOARD_X;
            case Engine::IAL::KeyCode::Y:
                return KEYBOARD_Y;
            case Engine::IAL::KeyCode::Z:
                return KEYBOARD_Z;

            case Engine::IAL::KeyCode::K0:
                return KEYBOARD_0;
            case Engine::IAL::KeyCode::K1:
                return KEYBOARD_1;
            case Engine::IAL::KeyCode::K2:
                return KEYBOARD_2;
            case Engine::IAL::KeyCode::K3:
                return KEYBOARD_3;
            case Engine::IAL::KeyCode::K4:
                return KEYBOARD_4;
            case Engine::IAL::KeyCode::K5:
                return KEYBOARD_5;
            case Engine::IAL::KeyCode::K6:
                return KEYBOARD_6;
            case Engine::IAL::KeyCode::K7:
                return KEYBOARD_7;
            case Engine::IAL::KeyCode::K8:
                return KEYBOARD_8;
            case Engine::IAL::KeyCode::K9:
                return KEYBOARD_9;

            case Engine::IAL::KeyCode::F1:
                return KEYBOARD_F1;
            case Engine::IAL::KeyCode::F2:
                return KEYBOARD_F2;
            case Engine::IAL::KeyCode::F3:
                return KEYBOARD_F3;
            case Engine::IAL::KeyCode::F4:
                return KEYBOARD_F4;
            case Engine::IAL::KeyCode::F5:
                return KEYBOARD_F5;
            case Engine::IAL::KeyCode::F6:
                return KEYBOARD_F6;
            case Engine::IAL::KeyCode::F7:
                return KEYBOARD_F7;
            case Engine::IAL::KeyCode::F8:
                return KEYBOARD_F8;
            case Engine::IAL::KeyCode::F9:
                return KEYBOARD_F9;
            case Engine::IAL::KeyCode::F10:
                return KEYBOARD_F10;
            case Engine::IAL::KeyCode::F11:
                return KEYBOARD_F11;
            case Engine::IAL::KeyCode::F12:
                return KEYBOARD_F12;

            case Engine::IAL::KeyCode::SPACE:
                return KEYBOARD_SPACE;
            case Engine::IAL::KeyCode::LEFT_SHIFT:
                return KEYBOARD_LSHIFT;
            case Engine::IAL::KeyCode::RIGHT_SHIFT:
                return KEYBOARD_RSHIFT;
            case Engine::IAL::KeyCode::LEFT_CTRL:
                return KEYBOARD_CONTROL;
            case Engine::IAL::KeyCode::RIGHT_CTRL:
                return KEYBOARD_CONTROL;
            case Engine::IAL::KeyCode::LEFT_ALT:
                return KEYBOARD_MENU;
            case Engine::IAL::KeyCode::RIGHT_ALT:
                return KEYBOARD_MENU;

            case Engine::IAL::KeyCode::TAB:
                return KEYBOARD_TAB;
            case Engine::IAL::KeyCode::ESCAPE:
                return KEYBOARD_ESCAPE;
            case Engine::IAL::KeyCode::ENTER:
                return KEYBOARD_RETURN;

            case Engine::IAL::KeyCode::UP:
                return KEYBOARD_UP;
            case Engine::IAL::KeyCode::DOWN:
                return KEYBOARD_DOWN;
            case Engine::IAL::KeyCode::LEFT:
                return KEYBOARD_LEFT;
            case Engine::IAL::KeyCode::RIGHT:
                return KEYBOARD_RIGHT;

            case Engine::IAL::KeyCode::UNKNOWN:
                return (KeyboardKeys)KEYBOARD_MAX;
            }
            return (KeyboardKeys)KEYBOARD_MAX;
        }


        MouseButtons MapButton(Engine::IAL::MouseButton b) {
            switch (b) {
            case Engine::IAL::MouseButton::LEFT:
                return MOUSE_LEFT;
            case Engine::IAL::MouseButton::RIGHT:
                return MOUSE_RIGHT;
            case Engine::IAL::MouseButton::MIDDLE:
                return MOUSE_MIDDLE;
            }
            return (MouseButtons)MOUSE_MAX;
        }

    }


    bool B_Keyboard::KeyDown(Engine::IAL::KeyCode key) {
        return m_nclKeyboard && m_nclKeyboard->KeyDown(MapKey(key));
    }

    bool B_Keyboard::KeyHeld(Engine::IAL::KeyCode key) {
        return m_nclKeyboard && m_nclKeyboard->KeyHeld(MapKey(key));
    }

    bool B_Keyboard::KeyTriggered(Engine::IAL::KeyCode key) {
        return m_nclKeyboard && m_nclKeyboard->KeyTriggered(MapKey(key));
    }


    ::Vector2 B_Mouse::GetRelativePosition() {
        return m_nclMouse ? m_nclMouse->GetRelativePosition() : ::Vector2(0, 0);
    }

    ::Vector2 B_Mouse::GetAbsolutePosition() {
        return m_nclMouse ? m_nclMouse->GetAbsolutePosition() : ::Vector2(0, 0);
    }

    bool B_Mouse::ButtonDown(Engine::IAL::MouseButton button) {
        return m_nclMouse && m_nclMouse->ButtonDown(MapButton(button));
    }

    bool B_Mouse::ButtonHeld(Engine::IAL::MouseButton button) {
        return m_nclMouse && m_nclMouse->ButtonHeld(MapButton(button));
    }

    bool B_Mouse::ButtonTriggered(Engine::IAL::MouseButton button) {
        if (!m_nclMouse) {
            return false;
        }
        auto mapped = MapButton(button);
        return m_nclMouse->ButtonDown(mapped) && !m_nclMouse->ButtonHeld(mapped);
    }

    bool B_Mouse::ButtonDoubleClicked(Engine::IAL::MouseButton button) {
        return m_nclMouse && m_nclMouse->DoubleClicked(MapButton(button));
    }

    bool B_Mouse::WheelMoved() {
        return m_nclMouse && m_nclMouse->WheelMoved();
    }

    int B_Mouse::GetWheelMovement() {
        return m_nclMouse ? m_nclMouse->GetWheelMovement() : 0;
    }


}
