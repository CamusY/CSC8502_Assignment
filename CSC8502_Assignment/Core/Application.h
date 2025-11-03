/*
* 文件: src/CSC8502_Demo/Core/Application.h
 * * 规范：
 * - NFR-1 / NFR-2: 
 * 此文件严禁 #include 任何 "nclgl/Window.h", "nclgl/OGLRenderer.h" 或 "Implementations/..."。
 * - 它只依赖 Engine/IAL/ 目录下的接口和 "nclgl/" 下的数学库。
 * - NFR-9: 
 * 此类通过构造函数接收所有系统服务的 IAL 接口。
 */

#pragma once

#include <memory>

// --- IAL 接口依赖 ---
//
#include "IAL/I_WindowSystem.h"
#include "IAL/I_ResourceFactory.h"
#include "IAL/I_DebugUI.h"
#include "IAL/I_InputDevice.h" //
#include "IAL/I_GameTimer.h"   //

// --- nclgl 数学库依赖 ---
// (按需包含，例如 Camera.h 会需要它们)

// --- Demo 层前向声明 ---
// (这些是 V13 计划 Day 4 的任务，我们先声明)
//class Renderer;
//class SceneManager;

class Application {
public:
    /**
     * @brief 构造函数 (依赖注入)
     * @details
     * - NFR-9: 
     * 接收来自 main.cpp 
     * 实例化的具体实现（例如 B_WindowSystem），
     * 但只保存它们的 IAL 接口。
     */
    Application(
        std::shared_ptr<Engine::IAL::I_WindowSystem> windowSystem,
        std::shared_ptr<Engine::IAL::I_ResourceFactory> resourceFactory,
        std::shared_ptr<Engine::IAL::I_DebugUI> debugUI
    );
    ~Application();

    /**
     * @brief 运行主循环
     * @details
     * - 循环调用 Update() 和 Render()，
     * 直到 I_WindowSystem::UpdateWindow() 
     * 返回 false。
     */
    void Run();

private:
    /**
     * @brief 更新逻辑（每帧调用）
     * @param dt 帧时间增量（来自 I_GameTimer）
     */
    void Update(float dt);

    /**
     * @brief 渲染逻辑（每帧调用）
     */
    void Render();

    // --- 注入的 IAL 系统服务 ---
    std::shared_ptr<Engine::IAL::I_WindowSystem> m_windowSystem;
    std::shared_ptr<Engine::IAL::I_ResourceFactory> m_resourceFactory;
    std::shared_ptr<Engine::IAL::I_DebugUI> m_debugUI;

    // --- IAL 子系统（从 I_WindowSystem 获取）---
    Engine::IAL::I_Keyboard* m_keyboard; // 非拥有指针
    Engine::IAL::I_Mouse* m_mouse;       // 非拥有指针
    Engine::IAL::I_GameTimer* m_timer;     // 非拥有指针

    // --- Demo 核心模块 (V13 Day 4 任务) ---
    // (Application 拥有这些模块)
//    std::unique_ptr<Renderer> m_renderer;
//    std::unique_ptr<SceneManager> m_sceneManager;

    bool m_isExiting;
};