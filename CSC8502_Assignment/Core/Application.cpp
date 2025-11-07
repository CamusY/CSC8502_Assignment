/**
* @file Application.cpp
 * @brief 应用程序核心类实现源文件。
 *
 * 本文件实现了 Application 类的构造函数和主循环。
 * 它依赖于 IAL 接口的具体定义来调用核心系统功能。
 *
 * 在 Day 2 阶段，Run() 函数仅包含最基础的窗口更新循环，用于验证架构的连通性。
 * 随着后续开发 (Sprint 1 后期)，此循环将加入计时器更新、场景更新和渲染器调用。
 */
#include "Application.h"
#include "IAL/I_WindowSystem.h"
#include "IAL/I_ResourceFactory.h"
#include "IAL/I_DebugUI.h"

Application::Application(std::shared_ptr<Engine::IAL::I_WindowSystem> window,
                         std::shared_ptr<Engine::IAL::I_ResourceFactory> factory,
                         std::shared_ptr<Engine::IAL::I_DebugUI> ui)
    : m_window(window), m_factory(factory), m_ui(ui) {
}

Application::~Application() {
}

void Application::Run() {
    while (m_window->UpdateWindow()) {
        m_ui->NewFrame();
        m_ui->Render();
        m_window->SwapBuffers();
    }
}