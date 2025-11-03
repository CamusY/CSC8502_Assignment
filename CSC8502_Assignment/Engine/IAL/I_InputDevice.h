#pragma once

// NFR-2: 接口直接依赖 nclgl 通用数学库 (Vector2.h)
// NFR-1: 严禁 #include "nclgl/Keyboard.h" 或 "nclgl/Mouse.h"
#include "nclgl/Vector2.h"

// 规范 4.1: 所有 IAL 接口均定义在 Engine::IAL 命名空间下
namespace Engine::IAL {

    /**
     * @brief 平台无关的键盘按键代码。
     * @details
     * - NFR-1: 此枚举是必须的，用于解耦 Demo 层与 nclgl::KeyboardKeys。
     * - B_Keyboard 适配器 (轨道 B) 的职责就是将 nclgl::KeyboardKeys 映射到此枚举。
     * - C_Keyboard 适配器 (轨道 C) 的职责就是将 GLFW_KEY_... 映射到此枚举。
     * - 基于 V13 计划 4.1 节 和 nclgl/Keyboard.h。
     */
    enum class KeyCode {
        UNKNOWN,

        W, A, S, D, SPACE,
        F, T, 
        LEFT_SHIFT, LEFT_CTRL,
        UP, DOWN, LEFT, RIGHT,
        ESCAPE,
        
        RETURN,
        TAB,
        
        K_0, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9,
        
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

        NUMPAD_0, NUMPAD_1, NUMPAD_2, NUMPAD_3, NUMPAD_4,
        NUMPAD_5, NUMPAD_6, NUMPAD_7, NUMPAD_8, NUMPAD_9,
        NUMPAD_ADD, NUMPAD_SUBTRACT, NUMPAD_MULTIPLY, NUMPAD_DIVIDE,
        NUMPAD_ENTER, 
        NUMPAD_DECIMAL,
        
        LCONTROL,
        RCONTROL,
        LSHIFT,
        RSHIFT,
        LMENU,
        RMENU
    };

    /**
     * @brief 平台无关的鼠标按键代码。
     * @details 基于 V13 计划 4.1 节 和 nclgl/Mouse.h (MouseButtons)。
     */
    enum class MouseButton {
        LEFT,
        RIGHT,
        MIDDLE,
        FOUR,
        FIVE
    };

    /**
     * @brief IAL 键盘接口
     * @details
     * - 抽象了 nclgl::Keyboard 的所有核心功能。
     * - Demo 层 (Application, Camera) 将只依赖此接口。
     * - KeyDown: 检查按键是否当前被按下 (抽象 nclgl::Keyboard::KeyDown)。
     * - KeyHeld: 检查按键是否已持续按下 (抽象 nclgl::Keyboard::KeyHeld)。
     * - KeyTriggered: 检查按键是否在本帧被首次按下 (抽象 nclgl::Keyboard::KeyTriggered)。
     */
    class I_Keyboard {
    public:
        virtual ~I_Keyboard() {}
        
        virtual bool KeyDown(KeyCode key) = 0; 
        virtual bool KeyHeld(KeyCode key) = 0;
        virtual bool KeyTriggered(KeyCode key) = 0;
    };

    /**
     * @brief IAL 鼠标接口
     * @details
     * - 抽象了 nclgl::Mouse 的所有核心功能。
     * - Demo 层 (Application, Camera) 将只依赖此接口。
     * - GetRelativePosition: 获取鼠标自上一帧以来的相对移动 (抽象 nclgl::Mouse::GetRelativePosition)。
     * - GetAbsolutePosition: 获取鼠标在窗口内的绝对坐标 (抽象 nclgl::Mouse::GetAbsolutePosition)。
     * - ButtonDown: 检查鼠标按键是否当前被按下 (抽象 nclgl::Mouse::ButtonDown)。
     * - ButtonHeld: 检查鼠标按键是否已持续按下 (抽象 nclgl::Mouse::ButtonHeld)。
     * - ButtonTriggered: 检查鼠标按键是否在本帧被首次按下 (nclgl::Mouse 中缺失，但为逻辑完整性而添加)。
     * - DoubleClicked: 检查鼠标按键是否在本帧被双击 (抽象 nclgl::Mouse::DoubleClicked)。
     * - WheelMoved: 检查滚轮是否在本帧滚动过 (抽象 nclgl::Mouse::WheelMoved)。
     * - GetWheelMovement: 获取滚轮滚动量 (抽象 nclgl::Mouse::GetWheelMovement)。
     */
    class I_Mouse {
    public:
        virtual ~I_Mouse() {}
        
        virtual Vector2 GetRelativePosition() = 0;
        virtual Vector2 GetAbsolutePosition() = 0;

        virtual bool ButtonDown(MouseButton button) = 0; 
        virtual bool ButtonHeld(MouseButton button) = 0;
        virtual bool ButtonTriggered(MouseButton button) = 0;
        virtual bool DoubleClicked(MouseButton button) = 0;
        
        virtual bool WheelMoved() = 0;
        virtual int GetWheelMovement() = 0;
    };

} // namespace Engine::IAL