#include "B_InputDevice.h"

#include "nclgl/Keyboard.h"
#include "nclgl/Mouse.h"
#include <stdexcept>


namespace NCLGL_Impl {

    // --- B_Keyboard 实现 ---

    B_Keyboard::B_Keyboard(Keyboard* nclKeyboard)
        : m_nclKeyboard(nclKeyboard) {
        if (!m_nclKeyboard) {
            // NFR-11.4 的关键风险点
            throw std::runtime_error("B_GameTimer construction failed: the passed-in nclTimer pointer is null.");
        }
    }

    bool B_Keyboard::KeyDown(Engine::IAL::KeyCode key) {
        //
        return m_nclKeyboard->KeyDown(ConvertKey(key));
    }

    bool B_Keyboard::KeyHeld(Engine::IAL::KeyCode key) {
        //
        return m_nclKeyboard->KeyHeld(ConvertKey(key));
    }

    bool B_Keyboard::KeyTriggered(Engine::IAL::KeyCode key) {
        //
        return m_nclKeyboard->KeyTriggered(ConvertKey(key));
    }

    KeyboardKeys B_Keyboard::ConvertKey(Engine::IAL::KeyCode key) const {
        // 这是适配器模式的核心：将 IAL 抽象键码映射到 nclgl 具体键码
        switch (key) {
            case Engine::IAL::KeyCode::W:           return KEYBOARD_W;
            case Engine::IAL::KeyCode::A:           return KEYBOARD_A;
            case Engine::IAL::KeyCode::S:           return KEYBOARD_S;
            case Engine::IAL::KeyCode::D:           return KEYBOARD_D;
            case Engine::IAL::KeyCode::SPACE:       return KEYBOARD_SPACE;
            case Engine::IAL::KeyCode::F:           return KEYBOARD_F;
            case Engine::IAL::KeyCode::T:           return KEYBOARD_T;
            case Engine::IAL::KeyCode::LEFT_SHIFT:  return KEYBOARD_LSHIFT;
            case Engine::IAL::KeyCode::LEFT_CTRL:   return KEYBOARD_LCONTROL;
            case Engine::IAL::KeyCode::UP:          return KEYBOARD_UP;
            case Engine::IAL::KeyCode::DOWN:        return KEYBOARD_DOWN;
            case Engine::IAL::KeyCode::LEFT:        return KEYBOARD_LEFT;
            case Engine::IAL::KeyCode::RIGHT:       return KEYBOARD_RIGHT;
            case Engine::IAL::KeyCode::ESCAPE:      return KEYBOARD_ESCAPE;
            case Engine::IAL::KeyCode::RETURN:      return KEYBOARD_RETURN;
            case Engine::IAL::KeyCode::TAB:         return KEYBOARD_TAB;
            case Engine::IAL::KeyCode::K_0:         return KEYBOARD_0;
            case Engine::IAL::KeyCode::K_1:         return KEYBOARD_1;
            case Engine::IAL::KeyCode::K_2:         return KEYBOARD_2;
            case Engine::IAL::KeyCode::K_3:         return KEYBOARD_3;
            case Engine::IAL::KeyCode::K_4:         return KEYBOARD_4;
            case Engine::IAL::KeyCode::K_5:         return KEYBOARD_5;
            case Engine::IAL::KeyCode::K_6:         return KEYBOARD_6;
            case Engine::IAL::KeyCode::K_7:         return KEYBOARD_7;
            case Engine::IAL::KeyCode::K_8:         return KEYBOARD_8;
            case Engine::IAL::KeyCode::K_9:         return KEYBOARD_9;
            case Engine::IAL::KeyCode::F1:          return KEYBOARD_F1;
            case Engine::IAL::KeyCode::F2:          return KEYBOARD_F2;
            case Engine::IAL::KeyCode::F3:          return KEYBOARD_F3;
            case Engine::IAL::KeyCode::F4:          return KEYBOARD_F4;
            case Engine::IAL::KeyCode::F5:          return KEYBOARD_F5;
            case Engine::IAL::KeyCode::F6:          return KEYBOARD_F6;
            case Engine::IAL::KeyCode::F7:          return KEYBOARD_F7;
            case Engine::IAL::KeyCode::F8:          return KEYBOARD_F8;
            case Engine::IAL::KeyCode::F9:          return KEYBOARD_F9;
            case Engine::IAL::KeyCode::F10:         return KEYBOARD_F10;
            case Engine::IAL::KeyCode::F11:         return KEYBOARD_F11;
            case Engine::IAL::KeyCode::F12:         return KEYBOARD_F12;
            case Engine::IAL::KeyCode::NUMPAD_0:    return KEYBOARD_NUMPAD0;
            case Engine::IAL::KeyCode::NUMPAD_1:    return KEYBOARD_NUMPAD1;
            case Engine::IAL::KeyCode::NUMPAD_2:    return KEYBOARD_NUMPAD2;
            case Engine::IAL::KeyCode::NUMPAD_3:    return KEYBOARD_NUMPAD3;
            case Engine::IAL::KeyCode::NUMPAD_4:    return KEYBOARD_NUMPAD4;
            case Engine::IAL::KeyCode::NUMPAD_5:    return KEYBOARD_NUMPAD5;
            case Engine::IAL::KeyCode::NUMPAD_6:    return KEYBOARD_NUMPAD6;
            case Engine::IAL::KeyCode::NUMPAD_7:    return KEYBOARD_NUMPAD7;
            case Engine::IAL::KeyCode::NUMPAD_8:    return KEYBOARD_NUMPAD8;
            case Engine::IAL::KeyCode::NUMPAD_9:    return KEYBOARD_NUMPAD9;
            case Engine::IAL::KeyCode::NUMPAD_ADD:  return KEYBOARD_ADD;
            case Engine::IAL::KeyCode::NUMPAD_SUBTRACT: return KEYBOARD_SUBTRACT;
            case Engine::IAL::KeyCode::NUMPAD_MULTIPLY: return KEYBOARD_MULTIPLY;
            case Engine::IAL::KeyCode::NUMPAD_DIVIDE:   return KEYBOARD_DIVIDE;
            case Engine::IAL::KeyCode::NUMPAD_ENTER:    return KEYBOARD_RETURN; // 注意：nclgl 没有 NUMPAD_ENTER，映射到 RETURN
            case Engine::IAL::KeyCode::NUMPAD_DECIMAL:  return KEYBOARD_DECIMAL;
            case Engine::IAL::KeyCode::LCONTROL:    return KEYBOARD_LCONTROL;
            case Engine::IAL::KeyCode::RCONTROL:    return KEYBOARD_RCONTROL;
            case Engine::IAL::KeyCode::LSHIFT:      return KEYBOARD_LSHIFT;
            case Engine::IAL::KeyCode::RSHIFT:      return KEYBOARD_RSHIFT;
            case Engine::IAL::KeyCode::LMENU:       return KEYBOARD_LMENU;
            case Engine::IAL::KeyCode::RMENU:       return KEYBOARD_RMENU;
            
            default:
            case Engine::IAL::KeyCode::UNKNOWN:     return (KeyboardKeys)0; // 返回一个安全的默认值
        }
    }


    // --- B_Mouse 实现 ---

    B_Mouse::B_Mouse(Mouse* nclMouse)
        : m_nclMouse(nclMouse) {
        if (!m_nclMouse) {
            // NFR-11.4 的关键风险点
            throw std::runtime_error("B_Mouse 构造失败：传入的 nclMouse 为空指针。");
        }
    }

    Vector2 B_Mouse::GetRelativePosition() {
        // NFR-2: nclgl::Mouse::GetRelativePosition()
        // 和 IAL::I_Mouse::GetRelativePosition()
        // 都使用 nclgl::Vector2，因此可以直接传递。
        return m_nclMouse->GetRelativePosition();
    }

    Vector2 B_Mouse::GetAbsolutePosition() {
        return m_nclMouse->GetAbsolutePosition();
    }

    bool B_Mouse::ButtonDown(Engine::IAL::MouseButton button) {
        return m_nclMouse->ButtonDown(ConvertButton(button));
    }

    bool B_Mouse::ButtonHeld(Engine::IAL::MouseButton button) {
        return m_nclMouse->ButtonHeld(ConvertButton(button));
    }

    bool B_Mouse::ButtonTriggered(Engine::IAL::MouseButton button) {
        // 关键实现：IAL 规范要求了此功能，但 nclgl::Mouse 没有提供。
        // 逻辑：Triggered = 当前帧被按下 (Down) 且 上一帧没有被按下 (Not Held)。
        MouseButtons nclButton = ConvertButton(button);
        return m_nclMouse->ButtonDown(nclButton) && !m_nclMouse->ButtonHeld(nclButton);
    }

    bool B_Mouse::DoubleClicked(Engine::IAL::MouseButton button) {
        return m_nclMouse->DoubleClicked(ConvertButton(button));
    }

    bool B_Mouse::WheelMoved() {
        return m_nclMouse->WheelMoved();
    }

    int B_Mouse::GetWheelMovement() {
        return m_nclMouse->GetWheelMovement();
    }

    MouseButtons B_Mouse::ConvertButton(Engine::IAL::MouseButton button) const {
        //
        switch (button) {
            case Engine::IAL::MouseButton::LEFT:    return MOUSE_LEFT;
            case Engine::IAL::MouseButton::RIGHT:   return MOUSE_RIGHT;
            case Engine::IAL::MouseButton::MIDDLE:  return MOUSE_MIDDLE;
            case Engine::IAL::MouseButton::FOUR:    return MOUSE_FOUR;
            case Engine::IAL::MouseButton::FIVE:    return MOUSE_FIVE;
            default:                                return MOUSE_LEFT; // 安全默认值
        }
    }

} // namespace NCLGL_Impl