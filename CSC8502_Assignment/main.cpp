/*
 * 文件: CSC8502_Assignment/main.cpp
 * * 职责: (NFR-9)
 * 1. 作为项目唯一的依赖注入 (DI) 入口点。
 * 2. 根据预处理器宏 (NCL_USE_CUSTOM_IMPL) 切换轨道 B (nclgl) 和轨道 C (Custom)。
 * 3. 实例化核心系统 (I_WindowSystem) 和服务 (I_ResourceFactory, I_DebugUI)。
 * 4. 将纯 IAL 接口注入 Application。
 * 当前轨道 B 路径已经连接至真实的 nclgl 实现，调用链会完成底层系统初始化。
 */

#include <memory>
#include "Core/Application.h" // 纯净的，只依赖 IAL 和 nclgl-math

// IAL 接口 (纯净)
// 这些是 Day 1 创建的纯虚接口
#include "IAL/I_WindowSystem.h"
#include "IAL/I_ResourceFactory.h"
#include "IAL/I_DebugUI.h"

// --- 依赖注入切换 (NFR-4, NFR-9) ---
// 
// #define NCL_USE_CUSTOM_IMPL // 取消注释此行以切换到轨道 C (自研)

#ifdef NCL_USE_CUSTOM_IMPL
    // --- 轨道 C: 自研实现 ---
    // main.cpp 被授权包含具体实现 (NFR-1)
    #include "Implementations/Custom_Impl/C_WindowSystem.h"
    #include "Implementations/Custom_Impl/C_Factory.h"
    #include "Implementations/Custom_Impl/C_DebugUI.h"
#else
    // --- 轨道 B: nclgl 默认实现 ---
    // main.cpp 被授权包含具体实现 (NFR-1)
    // 当前依赖注入已切换到实际的 nclgl 运行时代码
    #include "Implementations/NCLGL_Impl/B_WindowSystem.h"
    #include "Implementations/NCLGL_Impl/B_Factory.h"
    #include "Implementations/NCLGL_Impl/B_DebugUI_Null.h"
#endif

// -----------------------------------------------------------------

int main() {
    
    // 1. 实例化核心系统 (Window, Input, Timer)
    //    代码根据 NCL_USE_CUSTOM_IMPL 宏在编译时选择
    
    std::shared_ptr<Engine::IAL::I_WindowSystem> windowSystem;

#ifdef NCL_USE_CUSTOM_IMPL
    // (轨道 C - 暂不实现)
    windowSystem = std::make_shared<Custom_Impl::C_WindowSystem>();
#else
    // 轨道 B (nclgl 真实实现已连接)
    // WindowSystem 现在会创建底层窗口资源并初始化平台层
    windowSystem = std::make_shared<NCLGL_Impl::B_WindowSystem>();
#endif

    // 2. 初始化核心系统
    //    真实的 nclgl 初始化将创建窗口并绑定输入/计时设备
    if (!windowSystem->Init("CSC8502 Assignment", 1280, 720, false)) { // 使用你的项目名称
        return -1;
    }

    // 3. 实例化依赖的服务 (Factory, UI)
    std::shared_ptr<Engine::IAL::I_ResourceFactory> resourceFactory;
    std::shared_ptr<Engine::IAL::I_DebugUI> debugUI;

#ifdef NCL_USE_CUSTOM_IMPL
    // (轨道 C - 暂不实现)
    resourceFactory = std::make_shared<Custom_Impl::C_Factory>();
    debugUI         = std::make_shared<Custom_Impl::C_DebugUI>();
#else
    // 轨道 B (nclgl 真实实现)
    // 工厂与调试 UI 会拉起 nclgl 的资源加载与可视化调试模块
    resourceFactory = std::make_shared<NCLGL_Impl::B_Factory>();
    debugUI         = std::make_shared<NCLGL_Impl::B_DebugUI_Null>(); // (NFR-11.3)
#endif
    
    //    此时调试 UI 会完成自身初始化并注册窗口回调
    debugUI->Init(windowSystem->GetHandle()); // 当前调用可获取有效窗口句柄供 UI 系统使用
    
    // 5. 注入 IAL 接口
    //    Application 对轨道 B 或 C 毫不知情，
    //    它只接收 Engine::IAL::... 纯接口。
    Application app(windowSystem, resourceFactory, debugUI);
    
    // 6. 运行主循环
    app.Run();
    
    // 7. 清理
    debugUI->Shutdown();
    windowSystem->Shutdown();
    
    return 0;
}