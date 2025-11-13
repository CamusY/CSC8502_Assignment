/**
 * @file Application.h
 * @brief 应用程序核心类定义。
 *
 * 本文件定义了 Application 类，它是整个程序的运行时核心。
 * 它负责持有核心系统接口 (WindowSystem, ResourceFactory, DebugUI) 的共享所有权，
 * 并管理应用程序的主循环 (Main Loop)。
 *
 * Application 类:
 * 作为一个高层协调者，它不包含任何平台相关的具体实现细节。
 * 它完全依赖于 Engine::IAL 命名空间下的纯虚接口。
 *
 * 构造函数 Application(...):
 * 采用依赖注入 (Dependency Injection) 模式。
 * 必须在 main.cpp 中实例化具体的系统实现，并通过此构造函数注入到 Application 中。
 * 参数 window: 窗口系统接口，用于控制窗口更新和缓冲区交换。
 * 参数 factory: 资源工厂接口，用于后续创建渲染资源。
 * 参数 ui: 调试 UI 接口，用于在主循环中驱动 UI 的帧更新和渲染。
 * 参数 surfaceWidth / surfaceHeight: 与窗口一致的渲染分辨率，用于初始化 Day6 的后期处理 FBO。
 * 在 Day 4 之后，构造函数还会创建 SceneManager 与 Renderer，以驱动场景更新与渲染。
 * Day 8 将进一步注入 Camera，利用 IAL 输入接口在主循环中驱动自由相机 (P-5)。
 *
 * 成员函数 Run():
 * 执行应用程序的主循环。
 * 顺序：
 * 1. 调用窗口的 UpdateWindow() 处理操作系统事件。
 * 2. 通过计时器获取 Delta Time 并驱动 SceneManager::Update()。
 * 3. 调用 Renderer::Render(deltaTime) 遍历场景图并提交 Draw 调用。
 * 4. 调用 DebugUI::NewFrame/Render 驱动调试界面。
 * 5. 调用 SwapBuffers() 呈现最终图像。
 * 当 I_WindowSystem::UpdateWindow() 返回 false 时，循环结束，程序退出。
 *
 * 成员变量 m_window, m_factory, m_ui:
 * 持有核心系统接口的 shared_ptr。使用 shared_ptr 确保了系统资源的生命周期
 * 至少与 Application 实例一样长。
 * 成员变量 m_sceneManager, m_renderer:
 * 分别负责场景管理与渲染逻辑，贯穿整个应用生命周期。
 */
#pragma once
#include <memory>

namespace Engine::IAL {
    class I_WindowSystem;
    class I_ResourceFactory;
    class I_DebugUI;
    class I_Keyboard;
    class I_Mouse;
}

class SceneManager;
class Renderer;
class Camera;

class Application {
public:
    Application(std::shared_ptr<Engine::IAL::I_WindowSystem> window,
                std::shared_ptr<Engine::IAL::I_ResourceFactory> factory,
                std::shared_ptr<Engine::IAL::I_DebugUI> ui,
                int surfaceWidth,
                int surfaceHeight);
    ~Application();

    void Run();

private:
    std::shared_ptr<Engine::IAL::I_WindowSystem> m_window;
    std::shared_ptr<Engine::IAL::I_ResourceFactory> m_factory;
    std::shared_ptr<Engine::IAL::I_DebugUI> m_ui;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<SceneManager> m_sceneManager;
    std::shared_ptr<Renderer> m_renderer;
    Engine::IAL::I_Keyboard* m_keyboard;
    Engine::IAL::I_Mouse* m_mouse;
    int m_surfaceWidth;
    int m_surfaceHeight;
};