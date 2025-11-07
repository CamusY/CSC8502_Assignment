/**
* @file B_WindowSystem.h
 * @brief 轨道 B (NCLGL_Impl) 的窗口系统接口实现。
 *
 * 本文件定义了 B_WindowSystem 类，它是 Engine::IAL::I_WindowSystem 接口在 nclgl 框架下的具体实现。
 * 它负责管理 nclgl::Window 的生命周期，并作为 OpenGL 渲染上下文的持有者。
 *
 * 注意：在 Day 2 阶段，此类仅为满足链接需求的空壳实现。
 * 在后续 (Day 3) 实现中，它必须继承 nclgl::OGLRenderer 以正确创建和管理 OpenGL 上下文。
 *
 * B_WindowSystem 类 (NCLGL_Impl::B_WindowSystem):
 * 继承自 Engine::IAL::I_WindowSystem 纯虚接口。
 *
 * 成员函数 Init:
 * 初始化窗口系统。在 Day 2 空壳中返回 false 以便让程序立即安全退出。
 *
 * 成员函数 Shutdown, UpdateWindow, SwapBuffers:
 * 窗口生命周期管理函数的空实现。
 *
 * 成员函数 GetHandle, GetTimer, GetKeyboard, GetMouse:
 * 资源获取函数的空实现，Day 2 阶段返回 nullptr。
 */
#pragma once
#include "IAL/I_WindowSystem.h"

namespace NCLGL_Impl {

    class B_WindowSystem : public Engine::IAL::I_WindowSystem {
    public:
        B_WindowSystem();
        ~B_WindowSystem() override;

        bool Init(const std::string& title, int sizeX, int sizeY, bool fullScreen) override;
        void Shutdown() override;

        bool UpdateWindow() override;
        void SwapBuffers() override;

        void* GetHandle() override;
        Engine::IAL::I_GameTimer* GetTimer() const override;
        Engine::IAL::I_Keyboard* GetKeyboard() const override;
        Engine::IAL::I_Mouse* GetMouse() const override;
    };

}