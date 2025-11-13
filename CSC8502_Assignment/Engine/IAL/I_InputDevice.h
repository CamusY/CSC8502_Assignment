/**
 * @file I_InputDevice.h
 * @brief 定义了平台无关的、抽象的输入设备接口（键盘和鼠标）。
 * @details
 * 该文件的设计目的是将核心应用层（CSC8502_Demo）与底层的具体输入实现（无论是 nclgl 还是自研）
 * 完全解耦。
 * 它通过定义一组纯虚接口和平台无关的枚举来实现这一点。
 *
 * (NFR-2) 规范要求：本文件被授权且必须包含 nclgl 数学库（如 Vector2.h），
 * 以便在函数签名中直接使用 nclgl 的具体数学类型。
 * (NFR-1) 规范约束：本文件严禁包含任何 nclgl 的非数学系统级头文件（如 nclgl/Keyboard.h 或 nclgl/Mouse.h）。
 *
 * @see I_WindowSystem::GetKeyboard()
 * @see I_WindowSystem::GetMouse()
 *
 * @class Engine::IAL::KeyCode
 * @brief 平台无关的键盘按键枚举。
 * @details
 * 此枚举抽象了 nclgl::KeyboardKeys 或其他后端（如 GLFW_KEY_...）的具体按键定义。
 * 适配器实现（如 B_Keyboard）负责将后端枚举映射到此 IAL 枚举。
 *
 * @class Engine::IAL::MouseButton
 * @brief 平台无关的鼠标按键枚举。
 * @details
 * 此枚举抽象了 nclgl::MouseButtons 的具体按键定义。
 *
 * @class Engine::IAL::I_Keyboard
 * @brief 键盘设备的纯虚接口。
 * @details
 * 提供查询键盘状态的功能。实例由 I_WindowSystem::GetKeyboard() 提供。
 *
 * @fn Engine::IAL::I_Keyboard::~I_Keyboard
 * @brief 虚拟析构函数。
 *
 * @fn Engine::IAL::I_Keyboard::KeyDown(KeyCode key)
 * @brief 查询一个键当前是否被按下（按下状态）。
 * @details 抽象了 nclgl::Keyboard::KeyDown。
 * @param key 要查询的平台无关按键（IAL::KeyCode）。
 * @return 如果该键当前被按住，则为 true；否则为 false。
 *
 * @fn Engine::IAL::I_Keyboard::KeyHeld(KeyCode key)
 * @brief 查询一个键是否处于“保持”状态（已按下一帧或多帧）。
 * @details 抽象了 nclgl::Keyboard::KeyHeld。
 * @param key 要查询的平台无关按键（IAL::KeyCode）。
 * @return 如果该键被按住超过一帧，则为 true；否则为 false。
 *
 * @fn Engine::IAL::I_Keyboard::KeyTriggered(KeyCode key)
 * @brief 查询一个键是否在本帧被“触发”（按下事件，即上升沿）。
 * @details 抽象了 nclgl::Keyboard::KeyTriggered。
 * @param key 要查询的平台无关按键（IAL::KeyCode）。
 * @return 如果该键在本帧刚被按下，则为 true；否则为 false。
 *
 * @class Engine::IAL::I_Mouse
 * @brief 鼠标设备的纯虚接口。
 * @details
 * 提供查询鼠标状态和位置的功能。实例由 I_WindowSystem::GetMouse() 提供。
 *
 * @fn Engine::IAL::I_Mouse::~I_Mouse
 * @brief 虚拟析构函数。
 *
 * @fn Engine::IAL::I_Mouse::GetRelativePosition
 * @brief 获取自上一帧以来鼠标的相对移动（用于自由相机 P-5）。
 * @details
 * 抽象了 nclgl::Mouse::GetRelativePosition。
 * (NFR-2) 此函数签名必须直接使用 nclgl::Vector2 类型。
 * @return nclgl::Vector2 类型的(dx, dy)移动向量。
 *
 * @fn Engine::IAL::I_Mouse::GetAbsolutePosition
 * @brief 获取鼠标在窗口内的绝对像素坐标。
 * @details
 * 抽象了 nclgl::Mouse::GetAbsolutePosition。
 * (NFR-2) 此函数签名必须直接使用 nclgl::Vector2 类型。
 * @return nclgl::Vector2 类型的(x, y)绝对坐标。
 *
 * @fn Engine::IAL::I_Mouse::ButtonDown(MouseButton button)
 * @brief 查询一个鼠标按键当前是否被按下（按下状态）。
 * @details 抽象了 nclgl::Mouse::ButtonDown。
 * @param button 要查询的平台无关按键（IAL::MouseButton）。
 * @return 如果该按键当前被按住，则为 true；否则为 false。
 *
 * @fn Engine::IAL::I_Mouse::ButtonHeld(MouseButton button)
 * @brief 查询一个鼠标按键是否处于“保持”状态。
 * @details 抽象了 nclgl::Mouse::ButtonHeld。
 * @param button 要查询的平台无关按键（IAL::MouseButton）。
 * @return 如果该按键被按住超过一帧，则为 true；否则为 false。
 *
 * @fn Engine::IAL::I_Mouse::ButtonTriggered(MouseButton button)
 * @brief 查询一个鼠标按键是否在本帧被“触发”（按下事件）。
 * @details 抽象了 nclgl::Mouse::ButtonTriggered。
 * @param button 要查询的平台无关按键（IAL::MouseButton）。
 * @return 如果该按键在本帧刚被按下，则为 true；否则为 false。
 *
 * @fn Engine::IAL::I_Mouse::ButtonDoubleClicked(MouseButton button)
 * @brief 查询一个鼠标按键是否在本帧被“双击”。
 * @details 抽象了 nclgl::Mouse::DoubleClick。
 * @param button 要查询的平台无关按键（IAL::MouseButton）。
 * @return 如果该按键在本帧被双击，则为 true；否则为 false。
 *
 * @fn Engine::IAL::I_Mouse::WheelMoved
 * @brief 查询鼠标滚轮在本帧是否有移动。
 * @details 抽象了 nclgl::Mouse::WheelMoved。
 * @return 如果滚轮在本帧有移动，则为 true；否则为 false。
 *
 * @fn Engine::IAL::I_Mouse::GetWheelMovement
 * @brief 获取鼠标滚轮在本帧的移动量。
 * @details 抽象了 nclgl::Mouse::GetWheelMovement。
 * @return int 类型的滚轮移动量（通常是正值或负值）。
 */

#pragma once

#include "nclgl/Vector2.h"

namespace Engine::IAL {
    enum class KeyCode {
        UNKNOWN,
        A, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        
        K0, K1, K2, K3, K4, K5, K6, K7, K8, K9,
        
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        
        SPACE,
        LEFT_SHIFT, LEFT_CTRL, LEFT_ALT,
        RIGHT_SHIFT, RIGHT_CTRL, RIGHT_ALT,
        
        TAB,
        ESCAPE,
        ENTER,
        
        UP, DOWN, LEFT, RIGHT
    };
    
    enum class MouseButton {
        LEFT,
        RIGHT,
        MIDDLE
    };
    
    class I_Keyboard {
    public:
        virtual ~I_Keyboard() {}
        
        virtual bool KeyDown(KeyCode key) = 0;
        virtual bool KeyHeld(KeyCode key) = 0;
        virtual bool KeyTriggered(KeyCode key) = 0;
    };
    
    class I_Mouse {
    public:
        virtual ~I_Mouse() {}
        
        virtual Vector2 GetRelativePosition() = 0;
        virtual Vector2 GetAbsolutePosition() = 0;

        virtual bool ButtonDown(MouseButton button) = 0; 
        virtual bool ButtonHeld(MouseButton button) = 0;
        virtual bool ButtonTriggered(MouseButton button) = 0;
        virtual bool ButtonDoubleClicked(MouseButton button) = 0;
        
        virtual bool WheelMoved() = 0;
        virtual int GetWheelMovement() = 0;
    };
    
}