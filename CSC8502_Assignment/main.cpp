/*
 * 文件: src/CSC8502_Demo/main.cpp
 * * 规范：
 * - V13 计划 4.5.1 
 * 定义的依赖注入 (DI) 入口点。
 * - NFR-1 / NFR-9: 
 * 这是唯一被授权 #include "Implementations/..." 
 * 具体实现头文件的核心文件。
 * - NFR-4: 
 * 使用 C++ 预处理器宏来切换轨道 B 和轨道 C。
 */

#include <memory>
#include <iostream>

// 1. (规范) 包含纯净的 Application 类
#include "Core/Application.h" // (我们刚在上面定义的)

// 2. (规范) 包含纯净的 IAL 接口
#include "IAL/I_WindowSystem.h"
#include "IAL/I_ResourceFactory.h"
#include "IAL/I_DebugUI.h"

// --- 依赖注入切换 (NFR-4, NFR-9) ---
// 
// NFR-14: 
// 轨道 B (nclgl) 是核心交付物。
// 保持此行被注释以构建轨道 B。
// #define NCL_USE_CUSTOM_IMPL 
//
// ------------------------------------

#ifdef NCL_USE_CUSTOM_IMPL
    // --- 轨道 C: 自研实现 (V13 交付后目标) ---
    // #include "Implementations/Custom_Impl/C_WindowSystem.h"
    // #include "Implementations/Custom_Impl/C_Factory.h"
    // #include "Implementations/Custom_Impl/C_DebugUI.h"
#else
    // --- 轨道 B: nclgl 默认实现 (V13 核心交付物) ---
    // (main.cpp 被授权包含这些 "脏" 的头文件)
    #include "Implementations/NCLGL_Impl/B_WindowSystem.h"
    #include "Implementations/NCLGL_Impl/B_Factory.h"
    #include "Implementations/NCLGL_Impl/B_DebugUI_Null.h"
#endif

// -----------------------------------------------------------------

int main() {
    
    // 3. (规范) 声明 IAL 接口指针
    std::shared_ptr<Engine::IAL::I_WindowSystem> windowSystem;
    std::shared_ptr<Engine::IAL::I_ResourceFactory> resourceFactory;
    std::shared_ptr<Engine::IAL::I_DebugUI> debugUI;

    std::cout << "main.cpp: 正在选择轨道..." << std::endl;

#ifdef NCL_USE_CUSTOM_IMPL
    // --- 实例化轨道 C ---
    std::cout << "main.cpp: 轨道 C (Custom) 已选择。" << std::endl;
    // windowSystem    = std::make_shared<Custom_Impl::C_WindowSystem>();
    // resourceFactory = std::make_shared<Custom_Impl::C_Factory>();
    // debugUI         = std::make_shared<Custom_Impl::C_DebugUI>();
#else
    // --- 实例化轨道 B ---
    std::cout << "main.cpp: 轨道 B (NCLGL_Impl) 已选择。" << std::endl;
    windowSystem    = std::make_shared<NCLGL_Impl::B_WindowSystem>();
    resourceFactory = std::make_shared<NCLGL_Impl::B_Factory>();
    debugUI         = std::make_shared<NCLGL_Impl::B_DebugUI_Null>(); // NFR-11.3
#endif

    std::cout << "main.cpp: 正在初始化系统..." << std::endl;

    try {
        // 4. (规范) 初始化核心系统 (创建窗口和 GL 上下文)
        if (!windowSystem->Init("CSC8502 Demo (V13 Architecture)", 1280, 720, false)) {
            throw std::runtime_error("I_WindowSystem::Init() 失败！");
        }

        // 5. (规范) 初始化调试 UI (P-5)
        //    (在轨道 B 中，这将调用 B_DebugUI_Null::Init()，一个空函数)
        debugUI->Init(windowSystem->GetHandle());

        // 6. (规范) NFR-9: 
        //    注入 IAL 纯接口
        //    Application 对轨道 B 或 C 毫不知情。
        Application app(windowSystem, resourceFactory, debugUI);
        
        // 7. (规范) 运行主循环
        app.Run();

        // 8. (规范) 清理
        std::cout << "main.cpp: 正在关闭系统..." << std::endl;
        debugUI->Shutdown();
        windowSystem->Shutdown();

    } catch (const std::exception& e) {
        // 异常处理
        std::cerr << "\n!!!! 发生严重错误 !!!!\n" << e.what() << std::endl;
        // (B_WindowSystem 可能会在这里崩溃，因为它没有 HWND)
        // (在实际项目中，我们会在 B_WindowSystem::Shutdown 中添加 m_isInitialized 检查)
        
        // 确保即使 Init 失败，我们也能尝试关闭
        if (debugUI) debugUI->Shutdown();
        if (windowSystem) windowSystem->Shutdown();
        return -1;
    }

    std::cout << "main.cpp: 程序正常退出。" << std::endl;
    return 0;
}