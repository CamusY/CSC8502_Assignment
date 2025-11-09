/**
* @file B_WindowSystem.cpp
 * @brief 轨道 B (NCLGL_Impl) 的窗口系统接口实现源文件。
 *
 * 本文件实现了 B_WindowSystem 类。
 */
#include "B_WindowSystem.h"
#include "B_GameTimer.h"
#include "B_InputDevice.h"

#include "nclgl/Window.h"
#include "nclgl/OGLRenderer.h"
#include <glad/glad.h>

namespace {
    class InternalRenderer final : public OGLRenderer {
    public:
        explicit InternalRenderer(Window& w) : OGLRenderer(w) { init = true; }
        void RenderScene() override {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
    };
} // anonymous

namespace NCLGL_Impl {

    B_WindowSystem::B_WindowSystem() = default;

    B_WindowSystem::~B_WindowSystem() {
        B_WindowSystem::Shutdown();
    }

    bool B_WindowSystem::Init(const std::string& title, int sizeX, int sizeY, bool fullScreen) {
        m_window = new ::Window(title, sizeX, sizeY, fullScreen);
        if (!m_window->HasInitialised()) {
            delete m_window;
            m_window = nullptr;
            return false;
        }

        m_renderer = new InternalRenderer(*m_window);
        if (!m_renderer->HasInitialised()) {
            Shutdown();
            return false;
        }

        // 绑定静态键鼠
        m_keyboard = new B_Keyboard(::Window::GetKeyboard());
        m_mouse = new B_Mouse(::Window::GetMouse());

        // 包装计时器，使窗口系统能够向外提供统一的 I_GameTimer 接口
        m_timer = new B_GameTimer();
        return true;
    }

    void B_WindowSystem::Shutdown() {
        delete m_keyboard; m_keyboard = nullptr;
        delete m_mouse;    m_mouse = nullptr;
        delete m_timer;    m_timer = nullptr;
        delete m_renderer; m_renderer = nullptr;
        delete m_window;   m_window = nullptr;
    }

    bool B_WindowSystem::UpdateWindow() {
        if (!m_window) return false;
        
        const bool updated = m_window->UpdateWindow();

        if (updated && m_timer) m_timer -> tick();

        return updated;
    }

    void B_WindowSystem::SwapBuffers() {
        if (m_renderer) {
            m_renderer->SwapBuffers();
        }
    }

    void* B_WindowSystem::GetHandle() {
        return m_window ? m_window->GetHandle() : nullptr;
    }

    Engine::IAL::I_GameTimer* B_WindowSystem::GetTimer() const {
        return m_timer;
    }

    Engine::IAL::I_Keyboard* B_WindowSystem::GetKeyboard() const {
        return m_keyboard;
    }

    Engine::IAL::I_Mouse* B_WindowSystem::GetMouse() const {
        return m_mouse;
    }


}