#pragma once

// NFR-2: 接口直接依赖 nclgl 通用数学库
// (此文件不需要 nclgl 数学类型)

// NFR-1: 严禁 #include "nclgl/GameTimer.h"

// 规范 4.1: 所有 IAL 接口均定义在 Engine::IAL 命名空间下
namespace Engine::IAL {

    /**
     * @brief IAL 游戏计时器接口
     * @details
     * - 抽象了 nclgl::GameTimer 的功能。
     * - Demo 层的 Application 主循环将依赖此接口来获取帧时间增量 (delta time)。
     * - GetTimeDeltaSeconds: 抽象 nclgl::GameTimer::GetTimeDeltaSeconds。
     */
    class I_GameTimer {
    public:
        virtual ~I_GameTimer() {}
        
        virtual float GetTimeDeltaSeconds() const = 0;
        
        // 注意：nclgl::GameTimer 还有一个 Tick() 方法。
        // 根据 V13 计划，我们假定 Tick() 将在 I_WindowSystem::UpdateWindow() 内部被调用，
        // 因此不需要在 IAL 层面暴露 Tick()。
    };

} // namespace Engine::IAL