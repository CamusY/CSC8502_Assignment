/**
* @file B_WindowSystem.cpp
 * @brief 轨道 B (NCLGL_Impl) 的窗口系统接口实现源文件。
 *
 * 本文件实现了 B_WindowSystem 类。
 * 在 Day 2 阶段，所有函数均为基础的空壳实现 (Stubs)，旨在验证架构的依赖注入能否成功编译链接。
 * Init 函数返回 false，这将导致 main.cpp 中的应用程序在启动后立即正常退出，符合 Day 2 的预期行为。
 */
#include "B_WindowSystem.h"

namespace NCLGL_Impl {

    B_WindowSystem::B_WindowSystem() {
    }

    B_WindowSystem::~B_WindowSystem() {
    }

    bool B_WindowSystem::Init(const std::string& title, int sizeX, int sizeY, bool fullScreen) {
        return false;
    }

    void B_WindowSystem::Shutdown() {
    }

    bool B_WindowSystem::UpdateWindow() {
        return false;
    }

    void B_WindowSystem::SwapBuffers() {
    }

    void* B_WindowSystem::GetHandle() {
        return nullptr;
    }

    Engine::IAL::I_GameTimer* B_WindowSystem::GetTimer() const {
        return nullptr;
    }

    Engine::IAL::I_Keyboard* B_WindowSystem::GetKeyboard() const {
        return nullptr;
    }

    Engine::IAL::I_Mouse* B_WindowSystem::GetMouse() const {
        return nullptr;
    }

}