/*
 * (Day 2)
 * 文件: src/CSC8502_Demo/main.cpp
 * (在你的项目中路径为: CSC8502_Assignment/main.cpp)
 * * 职责: (NFR-9)
 * 1. 作为项目唯一的依赖注入 (DI) 入口点。
 * 2. 根据预处理器宏 (NCL_USE_CUSTOM_IMPL) 切换轨道 B (nclgl) 和轨道 C (Custom)。
 * 3. 实例化核心系统 (I_WindowSystem) 和服务 (I_ResourceFactory, I_DebugUI)。
 * 4. 将纯 IAL 接口注入 Application。
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
    // (Day 2 暂时不需要实现这些，但 main.cpp 必须包含此逻辑)
    // main.cpp 被授权包含具体实现 (NFR-1)
    #include "Implementations/Custom_Impl/C_WindowSystem.h"
    #include "Implementations/Custom_Impl/C_Factory.h"
    #include "Implementations/Custom_Impl/C_DebugUI.h"
#else
    // --- 轨道 B: nclgl 默认实现 ---
    // main.cpp 被授权包含具体实现 (NFR-1)
    // (这些是 Day 2 需要创建的 "空壳实现" Stubs)
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
    // 轨道 B (Day 2 使用空壳)
    windowSystem = std::make_shared<NCLGL_Impl::B_WindowSystem>();
#endif

    // 2. 初始化核心系统
    //    (在 Day 2，B_WindowSystem::Init 只是一个空函数，但调用必须存在)
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
    // 轨道 B (Day 2 使用空壳)
    resourceFactory = std::make_shared<NCLGL_Impl::B_Factory>();
    debugUI         = std::make_shared<NCLGL_Impl::B_DebugUI_Null>(); // (NFR-11.3)
#endif
    
    // 4. 初始化依赖服务 (UI 需要 Window Handle)
    //    (在 Day 2，B_DebugUI_Null::Init 只是一个空函数)
    debugUI->Init(windowSystem->GetHandle()); // GetHandle() 在 Day 2 的空壳中应返回 nullptr

    // 5. 注入 IAL 接口
    //    Application (Day 2 创建) 对轨道 B 或 C 毫不知情，
    //    它只接收 Engine::IAL::... 纯接口。
    Application app(windowSystem, resourceFactory, debugUI);
    
    // 6. 运行主循环
    //    (在 Day 2，Application::Run 只是一个空函数，程序会立即退出)
    app.Run();
    
    // 7. 清理
    //    (在 Day 2，空壳的 Shutdown 函数不执行任何操作)
    debugUI->Shutdown();
    windowSystem->Shutdown();
    
    return 0;
}