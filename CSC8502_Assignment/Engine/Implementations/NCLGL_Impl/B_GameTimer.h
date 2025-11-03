#pragma once

#include "IAL/I_GameTimer.h" // 包含我们要实现的 IAL 接口

// 仅在头文件中使用前向声明，避免暴露 nclgl 的完整头文件
class GameTimer; 

namespace NCLGL_Impl {

    /**
     * @brief 轨道 B (nclgl) 计时器实现
     * @details
     * - 实现了 Engine::IAL::I_GameTimer 接口。
     * - 这是一个围绕 nclgl::GameTimer 的轻量级包装器。
     * - 实例由 B_WindowSystem 创建，并注入从 nclgl::Window::GetTimer() 获取的指针。
     */
    class B_GameTimer : public Engine::IAL::I_GameTimer {
    public:
        /**
         * @brief 构造函数
         * @param nclTimer 一个指向 nclgl::Window 所拥有的 GameTimer 实例的指针。
         */
        B_GameTimer(GameTimer* nclTimer);
        virtual ~B_GameTimer() override = default;

        // --- I_GameTimer 接口实现 ---
        
        /**
         * @brief 获取自上一帧以来的时间增量（秒）
         * @details 直接调用 nclgl::GameTimer::GetTimeDeltaSeconds()
         * @return float 时间增量（秒）
         */
        virtual float GetTimeDeltaSeconds() const override;

    private:
        GameTimer* m_nclTimer; // 指向 nclgl 实例的非拥有指针
    };

} // namespace NCLGL_Impl