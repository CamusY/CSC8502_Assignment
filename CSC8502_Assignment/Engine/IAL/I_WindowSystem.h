/**
 * @file I_WindowSystem.h
 * @brief 定义了核心应用系统（窗口、OpenGL上下文、输入和计时器）的抽象接口。
 * @details
 * 这是整个架构中最重要的系统接口之一，由 `main.cpp`
 * 实例化并注入到 `Application` 中。
 * * (NFR-11.4 关键设计)
 * 此接口的设计目的是同时抽象 `nclgl` 库中的两个类：
 * 1.  `nclgl::Window`：负责窗口创建 (`Init`)、消息循环 (`UpdateWindow`)
 * 和输入/计时器设备 (`GetKeyboard`, `GetMouse`, `GetTimer`)。
 * 2.  `nclgl::OGLRenderer`：负责 OpenGL 上下文管理和
 * 双缓冲交换 (`SwapBuffers`)。
 *
 * 轨道 B 的实现 `B_WindowSystem`
 * 将通过一个复杂的设计（例如，继承 `OGLRenderer` 并持有一个 `Window` 实例）
 * 来同时满足这两个类的功能，以解决 `nclgl` 的循环依赖风险 (NFR-11.4)。
 *
 * (NFR-1) 规范约束：本文件严禁包含 nclgl/Window.h 或
 * nclgl/OGLRenderer.h。
 *
 * @see main.cpp (NFR-9 依赖注入点)
 * @see I_InputDevice.h
 * @see I_GameTimer.h
 *
 * @class Engine::IAL::I_WindowSystem
 * @brief 核心窗口和渲染上下文系统的纯虚接口。
 * @details
 * (新增) 此接口代表一个唯一的、不可复制的系统资源（物理窗口和 GL 上下文）。
 * 因此，其复制和移动构造函数/赋值运算符均被显式删除，
 * 以强制实施“唯一所有权”语义。它只能通过（智能）指针持有和传递。
 *
 * @fn Engine::IAL::I_WindowSystem::~I_WindowSystem
 * @brief 虚拟析 cấu 函数。
 *
 * @fn Engine::IAL::I_WindowSystem::Init
 * @brief 初始化系统，创建窗口和 OpenGL 渲染上下文。
 * @param title 窗口标题栏文本。
 * @param sizeX 窗口宽度（像素）。
 * @param sizeY 窗口高度（像素）。
 * @param fullScreen 是否全屏。
 * @return 如果初始化成功，则为 true；否则为 false。
 *
 * @fn Engine::IAL::I_WindowSystem::Shutdown
 * @brief 关闭窗口，销毁 OpenGL 上下文，并清理资源。
 *
 * @fn Engine::IAL::I_WindowSystem::UpdateWindow
 * @brief 更新窗口消息循环。
 * @details
 * 这是应用层主循环的核心驱动函数。它负责处理操作系统的消息（如关闭、
 * 调整大小）并更新所有输入设备的状态。
 * 抽象了 `nclgl::Window::UpdateWindow`。
 * @return 如果窗口请求关闭（例如，用户点击了'X'按钮），则为 false；
 * 否则为 true，表示主循环应继续。
 *
 * @fn Engine::IAL::I_WindowSystem::SwapBuffers
 * @brief 交换前后端渲染缓冲区。
 * @details
 * 在所有渲染命令完成后，应用层主循环调用此函数以将渲染结果显示到屏幕上。
 * 抽象了 `nclgl::OGLRenderer::SwapBuffers`。
 *
 * @fn Engine::IAL::I_WindowSystem::GetHandle
 * @brief 获取原生窗口句柄。
 * @details
 * (NFR-9)
 * 这是一个有意的类型擦除，返回 `void*` 以避免在 IAL 层暴露
 * 具体的平台实现（如 HWND 或 GLFWwindow*）。
 * 此句柄的主要用途是传递给 `I_DebugUI::Init`。
 * @return 指向原生窗口句柄的 `void` 指针。
 *
 * @fn Engine::IAL::I_WindowSystem::GetTimer
 * @brief 获取与此窗口关联的游戏计时器。
 * @details
 * 抽象了 `nclgl::Window::GetTimer`。
 * 返回的指针生命周期由 `I_WindowSystem` 实例管理。
 * @return 指向 `I_GameTimer` 纯虚接口的常量指针。
 *
 * @fn Engine::IAL::I_WindowSystem::GetKeyboard
 * @brief 获取与此窗口关联的键盘设备。
 * @details
 * 抽象了 `nclgl::Window::GetKeyboard`。
 * (NFR-11.4)
 * 轨道 B 实现 `B_WindowSystem` 必须返回一个包装了
 * `nclgl` 自动创建的 `static Keyboard` 实例的适配器。
 * @return 指向 `I_Keyboard` 纯虚接口的常量指针。
 *
 * @fn Engine::IAL::I_WindowSystem::GetMouse
 * @brief 获取与此窗口关联的鼠标设备。
 * @details
 * 抽象了 `nclgl::Window::GetMouse`。
 * (NFR-11.4)
 * 轨道 B 实现必须返回一个包装了
 * `nclgl` 自动创建的 `static Mouse` 实例的适配器。
 * @return 指向 `I_Mouse` 纯虚接口的常量指针。
 */

#pragma once

#include <string>
#include <memory>

// IAL 依赖 IAL (纯净)
#include "IAL/I_InputDevice.h"
#include "IAL/I_GameTimer.h"

namespace Engine::IAL {
    class I_WindowSystem {
    public:
        I_WindowSystem(const I_WindowSystem&) = delete;
        I_WindowSystem& operator=(const I_WindowSystem&) = delete;
        I_WindowSystem(I_WindowSystem&&) = delete;
        I_WindowSystem& operator=(I_WindowSystem&&) = delete;

        virtual ~I_WindowSystem() {}

        virtual bool Init(const std::string& title, int sizeX, int sizeY, bool fullScreen) = 0;
        virtual void Shutdown() = 0;

        virtual bool UpdateWindow() = 0;

        virtual void SwapBuffers() = 0;

        virtual void* GetHandle() = 0;

        virtual I_GameTimer* GetTimer() const = 0;
        virtual I_Keyboard* GetKeyboard() const = 0;
        virtual I_Mouse* GetMouse() const = 0;

    protected:
        I_WindowSystem() {}
    };

}
