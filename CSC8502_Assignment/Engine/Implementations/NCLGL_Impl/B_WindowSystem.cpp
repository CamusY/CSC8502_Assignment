#include "B_WindowSystem.h"

// 9. (规范) 在 .cpp 中包含所有具体实现
#include "nclgl/Window.h"     //
#include "nclgl/Keyboard.h"   //
#include "nclgl/Mouse.h"      //
#include "nclgl/GameTimer.h"  //
#include "B_InputDevice.h"    // (B_Keyboard 和 B_Mouse 的头文件)
#include "B_GameTimer.h"
#include <stdexcept>

namespace NCLGL_Impl {

    // --- B_NclglRendererProxy 实现 ---

    B_NclglRendererProxy::B_NclglRendererProxy(Window& window)
        // 10. (规范) NFR-11.4 
        //     将 nclgl::Window 实例传递给 OGLRenderer 的构造函数
        : OGLRenderer(window) { //
        
        // OGLRenderer 构造函数会创建 GL 上下文
        if (!HasInitialised()) {
            throw std::runtime_error("B_NclglRendererProxy 失败：OGLRenderer 初始化失败。");
        }
    }
    
    void B_NclglRendererProxy::PerformSwapBuffers() {
        // 11. (规范) OGLRenderer::SwapBuffers() 
        //     是 public 的，可以直接调用
        SwapBuffers();
    }


    // --- B_WindowSystem 实现 ---

    B_WindowSystem::B_WindowSystem() 
        : m_isInitialized(false), m_screenSize(0,0) {}

    B_WindowSystem::~B_WindowSystem() {
        Shutdown();
    }

    bool B_WindowSystem::Init(const std::string& title, int sizeX, int sizeY, bool fullScreen) {
        if (m_isInitialized) {
            return true;
        }

        try {
            // 12. (规范) NFR-11.4 
            //     步骤 1: 创建 nclgl::Window。
            //     这将创建窗口句柄 (HWND) 
            //     并自动创建静态的 Keyboard 和 Mouse 实例。
            m_nclWindow = std::make_unique<Window>(title, sizeX, sizeY, fullScreen);
            if (!m_nclWindow->HasInitialised()) {
                throw std::runtime_error("nclgl::Window 初始化失败。");
            }
            
            // 13. (规范) NFR-11.4
            //     步骤 2: 创建 B_NclglRendererProxy，
            //     将 nclgl::Window 注入 OGLRenderer 的构造函数。
            //     这将创建 OpenGL 上下文。
            m_nclRendererProxy = std::make_unique<B_NclglRendererProxy>(*m_nclWindow);

            // 14. (规范) NFR-11.4
            //     步骤 3: 将 Renderer 设置回 Window，
            //     完成 nclgl 的循环依赖。
            m_nclWindow->SetRenderer(m_nclRendererProxy.get()); //

            // 15. (规范) NFR-11.4
            //     步骤 4: 从 nclgl::Window 获取自动创建的静态实例...
            GameTimer* nclTimer   = m_nclWindow->GetTimer();     //
            Keyboard* nclKeyboard = m_nclWindow->GetKeyboard(); //
            Mouse* nclMouse    = m_nclWindow->GetMouse();     //
            
            // 16. (规范) ...并将它们包装在我们的 IAL 适配器中。
            m_timerAdapter    = std::make_shared<B_GameTimer>(nclTimer);
            m_keyboardAdapter = std::make_shared<B_Keyboard>(nclKeyboard);
            m_mouseAdapter    = std::make_shared<B_Mouse>(nclMouse);

        } catch (const std::exception& e) {
            std::cerr << "B_WindowSystem::Init() 失败: " << e.what() << std::endl;
            Shutdown(); // 清理部分初始化的资源
            return false;
        }

        m_screenSize.x = (float)sizeX;
        m_screenSize.y = (float)sizeY;
        m_isInitialized = true;
        return true;
    }

    void B_WindowSystem::Shutdown() {
        // 17. (规范) 析构顺序很重要：
        // RendererProxy 依赖 Window，所以先销毁 RendererProxy
        m_nclRendererProxy.reset();
        m_nclWindow.reset();
        
        m_timerAdapter.reset();
        m_keyboardAdapter.reset();
        m_mouseAdapter.reset();

        m_isInitialized = false;
    }

    bool B_WindowSystem::UpdateWindow() {
        if (!m_isInitialized) return false;
        
        // 18. (规范) IAL::UpdateWindow
        //     调用 nclgl::Window::UpdateWindow
        //     这将轮询 Windows 消息并更新输入设备
        return m_nclWindow->UpdateWindow();
    }

    void B_WindowSystem::SwapBuffers() {
        if (!m_isInitialized) return;
        
        // 19. (规范) IAL::SwapBuffers
        //     调用 OGLRenderer::SwapBuffers
        m_nclRendererProxy->PerformSwapBuffers();
    }

    void* B_WindowSystem::GetHandle() {
        if (!m_isInitialized) return nullptr;
        // 20. (规范) IAL::GetHandle
        //     (用于 DebugUI) 
        //     调用 nclgl::Window::GetHandle
        return (void*)m_nclWindow->GetHandle();
    }

    Engine::IAL::I_GameTimer* B_WindowSystem::GetTimer() const {
        return m_timerAdapter.get();
    }

    Engine::IAL::I_Keyboard* B_WindowSystem::GetKeyboard() const {
        return m_keyboardAdapter.get();
    }

    Engine::IAL::I_Mouse* B_WindowSystem::GetMouse() const {
        return m_mouseAdapter.get();
    }
    
    Vector2 B_WindowSystem::GetScreenSize() const {
        return m_screenSize;
    }

} // namespace NCLGL_Impl