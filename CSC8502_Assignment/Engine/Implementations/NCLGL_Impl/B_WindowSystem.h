/**
* @file B_WindowSystem.h
 * @brief 轨道 B (NCLGL_Impl) 的窗口系统接口实现。
 *
 * 本文件定义了 B_WindowSystem 类，它是 Engine::IAL::I_WindowSystem 接口在 nclgl 框架下的具体实现。
 * 它负责管理 nclgl::Window 的生命周期，并作为 OpenGL 渲染上下文的持有者。
 *
 * B_WindowSystem 类 (NCLGL_Impl::B_WindowSystem):
 * 继承自 Engine::IAL::I_WindowSystem 纯虚接口。
 *
 * 成员函数 Init:
 * 创建 nclgl::Window 与内部 OGLRenderer，完成 OpenGL 渲染上下文初始化，并实例化键鼠与计时器包装器。
 *
 * 成员函数 Shutdown:
 * 负责释放窗口、渲染器及其包装的输入/计时资源，确保没有资源泄露。
 *
 * 成员函数 UpdateWindow, SwapBuffers:
 * 分别委托 nclgl::Window::UpdateWindow 与 OGLRenderer::SwapBuffers 完成事件轮询与前后缓冲交换。
 *
 * 成员函数 GetHandle, GetTimer, GetKeyboard, GetMouse:
 * 返回底层窗口句柄及针对键盘、鼠标、计时器的适配器实例，这些包装器会始终提供有效对象而非空指针。
 */
#pragma once
#include "IAL/I_WindowSystem.h"


class Window;
class OGLRenderer;

namespace NCLGL_Impl {
    class B_GameTimer;
    class B_Keyboard;
    class B_Mouse;

    class B_WindowSystem : public Engine::IAL::I_WindowSystem {
    public:
        B_WindowSystem();
        ~B_WindowSystem() override;

        B_WindowSystem(const B_WindowSystem&) = delete;
        B_WindowSystem& operator=(const B_WindowSystem&) = delete;
        B_WindowSystem(B_WindowSystem&&) = delete;
        B_WindowSystem& operator=(B_WindowSystem&&) = delete;

        bool Init(const std::string& title, int sizeX, int sizeY, bool fullScreen) override;
        void Shutdown() override;

        bool UpdateWindow() override;
        void SwapBuffers() override;

        void* GetHandle() override;
        Engine::IAL::I_GameTimer* GetTimer() const override;
        Engine::IAL::I_Keyboard* GetKeyboard() const override;
        Engine::IAL::I_Mouse* GetMouse() const override;
        
    private:

        ::Window* m_window = nullptr;
        ::OGLRenderer* m_renderer = nullptr;

        B_GameTimer* m_timer = nullptr;
        B_Keyboard* m_keyboard = nullptr;
        B_Mouse* m_mouse = nullptr;
    };
    

}
