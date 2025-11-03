/*
 * 文件: src/CSC8502_Demo/Core/Application.cpp
 * * 规范：
 * - 实现了 Application 类的逻辑。
 * - Day 2 
 * 的实现将包含主循环，但 Renderer 和 SceneManager 将暂时留空（或注释掉），
 * 等待 Day 4 再实现。
 */

#include "Application.h"
#include <iostream>

// (Day 4 任务：
//  取消注释这些文件)
// #include "Rendering/Renderer.h"
// #include "Core/SceneManager.h"

Application::Application(
    std::shared_ptr<Engine::IAL::I_WindowSystem> windowSystem,
    std::shared_ptr<Engine::IAL::I_ResourceFactory> resourceFactory,
    std::shared_ptr<Engine::IAL::I_DebugUI> debugUI
) : 
    m_windowSystem(windowSystem),
    m_resourceFactory(resourceFactory),
    m_debugUI(debugUI),
    m_isExiting(false) 
{
    std::cout << "Application: 正在初始化..." << std::endl;

    // 1. (规范) NFR-11.4 
    //    从 B_WindowSystem 适配器获取 IAL 子接口
    m_keyboard = m_windowSystem->GetKeyboard();
    m_mouse = m_windowSystem->GetMouse();
    m_timer = m_windowSystem->GetTimer();

    if (!m_keyboard || !m_mouse || !m_timer) {
        throw std::runtime_error("Application 构造失败：未能从 I_WindowSystem 获取输入或计时器。");
    }

    // 2. (规范) Day 4 任务
    //    在这里实例化 Renderer 和 SceneManager，
    //    并将 IAL 接口注入它们。
    //
    // m_renderer = std::make_unique<Renderer>(m_windowSystem, m_resourceFactory, ...);
    // m_sceneManager = std::make_unique<SceneManager>(m_resourceFactory, ...);
    
    std::cout << "Application: 初始化完成。" << std::endl;
}

Application::~Application() {
    std::cout << "Application: 正在关闭..." << std::endl;
    // 3. (规范) m_renderer 和 m_sceneManager (unique_ptr)
    //    会在这里被自动销毁。
    //    IAL 接口 (shared_ptr) 会被自动释放。
}

void Application::Run() {
    std::cout << "Application: 开始主循环..." << std::endl;

    // 4. (规范) V13 Day 4 
    //    的主循环逻辑
    //    m_windowSystem->UpdateWindow()
    //    负责轮询 Windows 消息并更新输入状态。
    while (m_windowSystem->UpdateWindow() && !m_isExiting) {
        
        // 5. (规范) 从 IAL::I_GameTimer 
        //    获取帧时间
        float dt = m_timer->GetTimeDeltaSeconds();

        // 6. (规范) 更新所有逻辑
        Update(dt);

        // 7. (规范) 渲染所有内容
        Render();

        // 8. (规范) 交换缓冲区
        m_windowSystem->SwapBuffers();
    }

    std::cout << "Application: 主循环结束。" << std::endl;
}

void Application::Update(float dt) {
    // 9. (规范) P-5 
    //    检查退出键
    if (m_keyboard->KeyTriggered(Engine::IAL::KeyCode::ESCAPE)) {
        m_isExiting = true;
    }

    // (Day 4 任务)
    // m_sceneManager->Update(dt);
    // m_camera->Update(dt); // Camera 将在 Day 8 实现
}

void Application::Render() {
    // (Day 4 任务)
    // m_renderer->Render();

    // (Day 13 任务)
    // m_debugUI->NewFrame();
    // {
    //     // 渲染 UI 窗口
    //     if (m_debugUI->BeginWindow("Debug")) {
    //         m_debugUI->Text("Hello, IAL!");
    //         m_debugUI->EndWindow();
    //     }
    // }
    // m_debugUI->Render();
}