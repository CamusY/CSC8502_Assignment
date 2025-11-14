/**
* @file Application.cpp
 * @brief 应用程序核心类实现源文件。
 *
 * 本文件实现了 Application 类的构造函数和主循环。
 * 它依赖于 IAL 接口的具体定义来调用核心系统功能。
 *
 * Day19 起，Run() 在循环开始阶段响应 F 键切换相机模式：
 * 默认以轨迹模式启动，按下 F 后转为自由模式，再次按下则回到轨迹模式。
 * 主循环仍按“窗口事件 → 场景更新 → 渲染 → UI → 交换缓冲区”的顺序执行。
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
    if (m_camera) {
        m_camera->SetMode(Camera::Mode::Track);
    }
    if (m_sceneManager) {
        m_renderer = std::make_shared<Renderer>(m_factory,
                                                m_sceneManager->GetSceneGraph(),
                                                m_camera,
                                                m_ui,
                                                m_surfaceWidth,
                                                m_surfaceHeight);
    }
    if (m_sceneManager && m_renderer) {
        m_sceneManager->BindRenderer(m_renderer);
    }
}

Application::~Application() = default;

void Application::Run() {
    if (!m_window) {
        return;
    }
    int frameCount = 0;
    float timeAccum = 0.0f;
    while (m_window->UpdateWindow()) {
        float deltaTime = 0.0f;
        auto timer = m_window->GetTimer();
        if (timer) {
            deltaTime = timer->GetTimeDeltaSeconds();
        }
        frameCount++;
        timeAccum += deltaTime;

        if (m_window) {
            int width = m_surfaceWidth;
            int height = m_surfaceHeight;
            m_window->GetWindowSize(width, height);
            if (width <= 0) {
                width = m_surfaceWidth;
            }
            if (height <= 0) {
                height = m_surfaceHeight;
            }
            if ((width != m_surfaceWidth || height != m_surfaceHeight) && m_renderer) {
                m_surfaceWidth = width;
                m_surfaceHeight = height;
                m_renderer->OnSurfaceResized(width, height);
            }
            else {
                m_surfaceWidth = width;
                m_surfaceHeight = height;
            }
        }

        if (timeAccum >= 1.0f) {
            std::cout << "FPS: " << static_cast<float>(frameCount) / timeAccum << '\n';
            frameCount = 0;
            timeAccum = 0.0f;
        }
        if (m_keyboard && m_window && m_keyboard->KeyTriggered(Engine::IAL::KeyCode::F11)) {
            const bool target = !m_window->IsFullScreen();
            if (m_window->SetFullScreen(target)) {
                int width = m_surfaceWidth;
                int height = m_surfaceHeight;
                m_window->GetWindowSize(width, height);
                if (width > 0 && height > 0 && m_renderer) {
                    m_surfaceWidth = width;
                    m_surfaceHeight = height;
                    m_renderer->OnSurfaceResized(width, height);
                }
            }
        }
        if (m_keyboard && m_renderer && m_keyboard->KeyTriggered(Engine::IAL::KeyCode::M)) {
            m_renderer->ToggleMultiViewLayout();
        }
        if (m_keyboard && m_renderer) {
            if (m_keyboard->KeyTriggered(Engine::IAL::KeyCode::K0)) {
                m_renderer->SetDefaultViewMode(Renderer::RenderDebugMode::Standard);
            }
            else if (m_keyboard->KeyTriggered(Engine::IAL::KeyCode::K1)) {
                m_renderer->SetDefaultViewMode(Renderer::RenderDebugMode::Wireframe);
            }
            else if (m_keyboard->KeyTriggered(Engine::IAL::KeyCode::K2)) {
                m_renderer->SetDefaultViewMode(Renderer::RenderDebugMode::Normal);
            }
            else if (m_keyboard->KeyTriggered(Engine::IAL::KeyCode::K3)) {
                m_renderer->SetDefaultViewMode(Renderer::RenderDebugMode::Depth);
            }
            else if (m_keyboard->KeyTriggered(Engine::IAL::KeyCode::K4)) {
                m_renderer->SetDefaultViewMode(Renderer::RenderDebugMode::Bloom);
            }
        }
        if (m_camera && m_keyboard && m_keyboard->KeyTriggered(Engine::IAL::KeyCode::F)) {
            const auto currentMode = m_camera->GetMode();
            if (currentMode == Camera::Mode::Track) {
                m_camera->SetMode(Camera::Mode::Free);
            } else {
                m_camera->SetMode(Camera::Mode::Track);
            }
        }
        if (m_camera) {
            m_camera->Update(deltaTime, m_keyboard, m_mouse);
        }
        if (m_sceneManager) {
            m_sceneManager->Update(deltaTime, m_keyboard);
        }
        if (m_ui) {
            m_ui->NewFrame();
        }
        if (m_renderer) {
            m_renderer->Render(deltaTime);
        }
        if (m_ui) {
            m_ui->Render();
        }
        m_window->SwapBuffers();
        if (m_keyboard && m_keyboard->KeyTriggered(Engine::IAL::KeyCode::ESCAPE)) { break; }

    }
}