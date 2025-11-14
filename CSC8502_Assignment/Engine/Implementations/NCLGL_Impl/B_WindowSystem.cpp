/**
 * @file B_WindowSystem.cpp
 * @brief 轨道 B (NCLGL_Impl) 的窗口系统接口实现源文件。
 *
 * 本文件实现了 B_WindowSystem 类，负责创建 nclgl 窗口与渲染器，
 * 并包装 nclgl 提供的静态键鼠以及轨道 B 的计时器以统一提供 IAL 接口。
 */
#include "B_WindowSystem.h"
#include "B_GameTimer.h"
#include "B_InputDevice.h"

#include "nclgl/Window.h"
#include "nclgl/OGLRenderer.h"
#include "nclgl/Vector2.h"
#include <glad/glad.h>

namespace {
    class InternalRenderer final : public OGLRenderer {
    public:
        explicit InternalRenderer(Window& w) :
            OGLRenderer(w) { init = true; }

        void RenderScene() override {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
    };
}

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

        m_isFullscreen = fullScreen;
        if (auto handle = m_window->GetHandle()) {
            GetWindowRect(static_cast<HWND>(handle), &m_windowedRect);
        }

        m_keyboard = new B_Keyboard(::Window::GetKeyboard());
        m_mouse = new B_Mouse(::Window::GetMouse());

        m_window->LockMouseToWindow(true);
        m_window->ShowOSPointer(false);

        m_timer = new B_GameTimer();
        return true;
    }

    void B_WindowSystem::Shutdown() {
        if (m_window) {
            m_window->LockMouseToWindow(false);
            m_window->ShowOSPointer(true);
        }
        delete m_keyboard;
        m_keyboard = nullptr;
        delete m_mouse;
        m_mouse = nullptr;
        delete m_timer;
        m_timer = nullptr;
        delete m_renderer;
        m_renderer = nullptr;
        delete m_window;
        m_window = nullptr;
        m_isFullscreen = false;
    }

    bool B_WindowSystem::UpdateWindow() {
        if (!m_window)
            return false;

        const bool updated = m_window->UpdateWindow();

        if (updated && m_timer)
            m_timer->tick();

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

    bool B_WindowSystem::SetFullScreen(bool enabled) {
        if (!m_window) {
            return false;
        }
        if (m_isFullscreen == enabled) {
            return true;
        }
        HWND handle = static_cast<HWND>(m_window->GetHandle());
        if (!handle) {
            return false;
        }
        DWORD style = GetWindowLong(handle, GWL_STYLE);
        if (enabled) {
            GetWindowRect(handle, &m_windowedRect);
            MONITORINFO info{};
            info.cbSize = sizeof(MONITORINFO);
            if (!GetMonitorInfo(MonitorFromWindow(handle, MONITOR_DEFAULTTOPRIMARY), &info)) {
                return false;
            }
            SetWindowLong(handle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(handle,
                         HWND_TOP,
                         info.rcMonitor.left,
                         info.rcMonitor.top,
                         info.rcMonitor.right - info.rcMonitor.left,
                         info.rcMonitor.bottom - info.rcMonitor.top,
                         SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
        }
        else {
            SetWindowLong(handle, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
            SetWindowPos(handle,
                         nullptr,
                         m_windowedRect.left,
                         m_windowedRect.top,
                         m_windowedRect.right - m_windowedRect.left,
                         m_windowedRect.bottom - m_windowedRect.top,
                         SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
        }
        m_isFullscreen = enabled;
        return true;
    }

    bool B_WindowSystem::IsFullScreen() const {
        return m_isFullscreen;
    }

    void B_WindowSystem::GetWindowSize(int& width, int& height) const {
        width = 0;
        height = 0;
        if (!m_window) {
            return;
        }
        const Vector2 size = m_window->GetScreenSize();
        width = static_cast<int>(size.x);
        height = static_cast<int>(size.y);
    }

}
