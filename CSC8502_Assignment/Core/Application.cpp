/**
 * @file Application.cpp
 * @brief 应用程序核心类实现源文件。
 *
 * 本文件实现了 Application 类的构造函数和主循环。
 * 它依赖于 IAL 接口的具体定义来调用核心系统功能。
 *
 * 在 Day 4 阶段，Run() 函数建立了完整的系统调用顺序：
 * 先更新窗口事件，再依据计时器驱动场景管理与渲染管线，最后刷新调试 UI 并交换缓冲区。
 */
#include "Application.h"
#include "IAL/I_WindowSystem.h"
#include "IAL/I_ResourceFactory.h"
#include "IAL/I_DebugUI.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "Camera.h"

Application::Application(std::shared_ptr<Engine::IAL::I_WindowSystem> window,
                         std::shared_ptr<Engine::IAL::I_ResourceFactory> factory,
                         std::shared_ptr<Engine::IAL::I_DebugUI> ui,
                         int surfaceWidth,
                         int surfaceHeight)
    : m_window(window)
    , m_factory(factory)
    , m_ui(ui)
    , m_camera(std::make_shared<Camera>())
    , m_sceneManager(std::make_shared<SceneManager>(factory))
    , m_renderer(nullptr)
    , m_keyboard(nullptr)
    , m_mouse(nullptr)
    , m_surfaceWidth(surfaceWidth)
    , m_surfaceHeight(surfaceHeight) {
    if (m_window) {
        m_keyboard = m_window->GetKeyboard();
        m_mouse = m_window->GetMouse();
    }
    if (m_sceneManager) {
        m_renderer = std::make_shared<Renderer>(m_factory,
                                                m_sceneManager->GetSceneGraph(),
                                                m_camera,
                                                m_surfaceWidth,
                                                m_surfaceHeight);
    }
}

Application::~Application() = default;

void Application::Run() {
    while (m_window->UpdateWindow()) {
        float deltaTime = 0.0f;
        auto timer = m_window->GetTimer();
        if (timer) {
            deltaTime = timer->GetTimeDeltaSeconds();
        }
        if (m_camera) {
            m_camera->Update(deltaTime, m_keyboard, m_mouse);
        }
        if (m_sceneManager) {
            m_sceneManager->Update(deltaTime);
        }
        if (m_renderer) {
            m_renderer->Render();
        }
        m_ui->NewFrame();
        m_ui->Render();
        m_window->SwapBuffers();
    }
}