#pragma once

#include "IAL/I_WindowSystem.h" // 1. (规范) 实现 IAL 接口
#include <memory>
#include <string>

// 2. (规范) 包含 nclgl 的 OGLRenderer 基类，因为我们需要
//    创建一个虚拟的子类来实现它
#include "nclgl/OGLRenderer.h" //

// 3. (规范) 前向声明 nclgl 类
class Window;
class Keyboard;
class Mouse;
class GameTimer;

// 4. (规范) 前向声明我们的适配器类
namespace NCLGL_Impl {
    class B_Keyboard;
    class B_Mouse;
    class B_GameTimer;

    /**
     * @brief NFR-11.4 
     * 解决方案：一个内部的、虚拟的 OGLRenderer 实现。
     * @details
     * - nclgl::Window::SetRenderer 
     * 需要一个 OGLRenderer 指针。
     * - OGLRenderer 是抽象基类。
     * - Demo 层的 Renderer (V13 Day 4) 
     * 是纯 IAL 的，不能继承 OGLRenderer。
     * - 因此，B_WindowSystem 必须提供一个满足 nclgl 依赖的虚拟实现。
     * - 它的 RenderScene() 将永远不会被调用。
     */
    class B_NclglRendererProxy : public OGLRenderer {
    public:
        B_NclglRendererProxy(Window& window);
        virtual ~B_NclglRendererProxy() {}
        
        // 5. (规范) 实现纯虚的 RenderScene
        virtual void RenderScene() override { /* 故意留空 */ }
        
        // 6. (规范) 暴露 SwapBuffers，以便 B_WindowSystem 可以调用
        void PerformSwapBuffers();
    };


    /**
     * @brief 轨道 B (nclgl) 核心系统实现
     * @details
     * - 实现了 Engine::IAL::I_WindowSystem 接口。
     * - NFR-11.4 
     * 高风险适配器：封装了 nclgl::Window 
     * 和 OGLRenderer
     * 的循环依赖。
     * - NFR-11.4 
     * 创建并包装由 nclgl::Window 创建的静态输入实例。
     */
    class B_WindowSystem : public Engine::IAL::I_WindowSystem {
    public:
        B_WindowSystem();
        virtual ~B_WindowSystem();

        // --- I_WindowSystem 接口实现 ---
        virtual bool Init(const std::string& title, int sizeX, int sizeY, bool fullScreen) override;
        virtual void Shutdown() override;
        
        virtual bool UpdateWindow() override;
        virtual void SwapBuffers() override;

        virtual void* GetHandle() override;

        virtual Engine::IAL::I_GameTimer* GetTimer() const override;
        virtual Engine::IAL::I_Keyboard* GetKeyboard() const override;
        virtual Engine::IAL::I_Mouse* GetMouse() const override;
        
        // (B_FrameBuffer 需要知道主屏幕尺寸以恢复视口)
        Vector2 GetScreenSize() const;

    private:
        // 7. (规范) B_WindowSystem 拥有 nclgl 实例
        std::unique_ptr<Window> m_nclWindow;
        std::unique_ptr<B_NclglRendererProxy> m_nclRendererProxy;

        // 8. (规范) B_WindowSystem 拥有 IAL 适配器实例
        std::shared_ptr<B_GameTimer> m_timerAdapter;
        std::shared_ptr<B_Keyboard> m_keyboardAdapter;
        std::shared_ptr<B_Mouse> m_mouseAdapter;
        
        Vector2 m_screenSize;
        bool m_isInitialized;
    };

} // namespace NCLGL_Impl