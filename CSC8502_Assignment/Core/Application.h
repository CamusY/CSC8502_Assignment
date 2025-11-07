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
 * 参数 factory: 资源工厂接口，用于后续创建渲染资源 (Day 2 暂未使用，但必须注入)。
 * 参数 ui: 调试 UI 接口，用于在主循环中驱动 UI 的帧更新和渲染。
 *
 * 成员函数 Run():
 * 执行应用程序的主循环。
 * 在 Day 2 阶段，它提供了一个最基础的循环框架，调用窗口的更新函数。
 * 当 I_WindowSystem::UpdateWindow() 返回 false 时，循环结束，程序退出。
 *
 * 成员变量 m_window, m_factory, m_ui:
 * 持有核心系统接口的 shared_ptr。使用 shared_ptr 确保了系统资源的生命周期
 * 至少与 Application 实例一样长。
 */
#pragma once
#include <memory>

namespace Engine::IAL {
    class I_WindowSystem;
    class I_ResourceFactory;
    class I_DebugUI;
}

class Application {
public:
    Application(std::shared_ptr<Engine::IAL::I_WindowSystem> window,
                std::shared_ptr<Engine::IAL::I_ResourceFactory> factory,
                std::shared_ptr<Engine::IAL::I_DebugUI> ui);
    ~Application();

    void Run();

private:
    std::shared_ptr<Engine::IAL::I_WindowSystem> m_window;
    std::shared_ptr<Engine::IAL::I_ResourceFactory> m_factory;
    std::shared_ptr<Engine::IAL::I_DebugUI> m_ui;
};