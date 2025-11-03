#pragma once

#include <string>
#include <memory> // 仅用于前向声明 (如果需要)

// IAL 依赖 IAL (纯净)
// 规范 4.1 示例中完整包含了头文件，而不是使用前向声明，我们遵循此规范
#include "IAL/I_InputDevice.h"
#include "IAL/I_GameTimer.h"

// NFR-2: 接口直接依赖 nclgl 通用数学库
// (此文件不需要 nclgl 数学类型，因为它只返回其他 IAL 接口)

// NFR-1: 严禁 #include "nclgl/Window.h" 或 "nclgl/OGLRenderer.h"

namespace Engine::IAL {
    
    // (前向声明，因为 I_InputDevice.h 和 I_GameTimer.h 已被包含)
    // class I_Keyboard;
    // class I_Mouse;
    // class I_GameTimer;

    /**
     * @brief IAL 核心系统接口 (P-0, P-5)
     * @details
     * - 这是一个高度复杂的抽象，它同时封装了 nclgl::Window 
     * 和 nclgl::OGLRenderer 的职责。
     * - NFR-11.4: 轨道 B 的 B_WindowSystem 实现将非常复杂，需要处理 nclgl 的循环依赖和静态输入实例。
     * - Demo 层的 Application 将依赖此接口来运行主循环和访问子系统。
     * - Init: 抽象窗口和 OpenGL 上下文的创建。
     * - UpdateWindow: 抽象 nclgl::Window::UpdateWindow，处理窗口消息和输入轮询。
     * - SwapBuffers: 抽象 nclgl::OGLRenderer::SwapBuffers。
     * - GetHandle: 抽象 nclgl::Window::GetHandle，返回一个 void* (HWND 或 GLFWwindow*) 以便注入 I_DebugUI。
     * - GetTimer/Keyboard/Mouse: 抽象 nclgl::Window::Get...，返回 IAL 接口。
     */
    class I_WindowSystem {
    public:
        virtual ~I_WindowSystem() = default;

        I_WindowSystem(const I_WindowSystem&) = delete;
        I_WindowSystem& operator=(const I_WindowSystem&) = delete;
        I_WindowSystem(I_WindowSystem&&) = delete;
        I_WindowSystem& operator=(I_WindowSystem&&) = delete;

        virtual bool Init(const std::string& title, int sizeX, int sizeY, bool fullScreen) = 0;
        virtual void Shutdown() = 0;
        
        virtual bool UpdateWindow() = 0; 
        virtual void SwapBuffers() = 0;

        virtual void* GetHandle() = 0;

        virtual I_GameTimer* GetTimer() const = 0;
        virtual I_Keyboard* GetKeyboard() const = 0;
        virtual I_Mouse* GetMouse() const = 0;

    protected:
        I_WindowSystem() = default;
    };
    

} // namespace Engine::IAL