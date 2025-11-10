/**
 * @file main.cpp
 * @brief 应用的依赖注入入口，封装轨道 B 与轨道 C 的运行时绑定。
 *
 * @details main 函数负责在编译期依据 `NCL_USE_CUSTOM_IMPL` 宏切换两条依赖注入路径。
 * 轨道 B 通过 nclgl 提供的窗口与资源实现完成真实平台初始化，再将 `I_WindowSystem`、
 * `I_ResourceFactory` 和 `I_DebugUI` 作为纯接口注入 `Application`。当轨道 B 处于启用状态时，
 * `B_DebugUI_Null` 会以 Null Object 身份出现，仅保持接口契约而不执行任何 UI 绘制逻辑，
 * 其存在用于占位，使业务在无调试 UI 的情况下仍符合依赖关系。轨道 C 的宏开关预留给自研
 * 实现，保持同样的接口注入流程，从而维持上下文无关的应用主循环。
 */

#include <memory>
#include "Core/Application.h"

#include "IAL/I_WindowSystem.h"
#include "IAL/I_ResourceFactory.h"
#include "IAL/I_DebugUI.h"

#ifdef NCL_USE_CUSTOM_IMPL
    #include "Implementations/Custom_Impl/C_WindowSystem.h"
    #include "Implementations/Custom_Impl/C_Factory.h"
    #include "Implementations/Custom_Impl/C_DebugUI.h"
#else
    #include "Implementations/NCLGL_Impl/B_WindowSystem.h"
    #include "Implementations/NCLGL_Impl/B_Factory.h"
    #include "Implementations/NCLGL_Impl/B_DebugUI_Null.h"
#endif

int main() {
    std::shared_ptr<Engine::IAL::I_WindowSystem> windowSystem;

#ifdef NCL_USE_CUSTOM_IMPL
    windowSystem = std::make_shared<Custom_Impl::C_WindowSystem>();
#else
    windowSystem = std::make_shared<NCLGL_Impl::B_WindowSystem>();
#endif

    constexpr int kWindowWidth = 1280;
    constexpr int kWindowHeight = 720;

    if (!windowSystem->Init("CSC8502 Assignment", kWindowWidth, kWindowHeight, false)) {
        return -1;
    }
    
    std::shared_ptr<Engine::IAL::I_ResourceFactory> resourceFactory;
    std::shared_ptr<Engine::IAL::I_DebugUI> debugUI;

#ifdef NCL_USE_CUSTOM_IMPL
    resourceFactory = std::make_shared<Custom_Impl::C_Factory>();
    debugUI         = std::make_shared<Custom_Impl::C_DebugUI>();
#else
    resourceFactory = std::make_shared<NCLGL_Impl::B_Factory>();
    debugUI         = std::make_shared<NCLGL_Impl::B_DebugUI_Null>();
#endif

    debugUI->Init(windowSystem->GetHandle());

    Application app(windowSystem, resourceFactory, debugUI, kWindowWidth, kWindowHeight);

    app.Run();

    debugUI->Shutdown();
    windowSystem->Shutdown();

    return 0;
}