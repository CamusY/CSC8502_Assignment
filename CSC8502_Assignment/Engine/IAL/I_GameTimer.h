/**
* @file I_GameTimer.h
 * @brief 定义了平台无关的、抽象的游戏计时器接口。
 * @details
 * 该文件的设计目的是为应用层（CSC8502_Demo）提供一个获取帧间增量时间（Delta Time）
 * 的统一方式，而无需关心计时器（如 nclgl::GameTimer）的具体实现。
 * 实例将由 I_WindowSystem::GetTimer() 提供。
 *
 * (NFR-1) 规范约束：本文件严禁包含 nclgl/GameTimer.h。
 *
 * @class Engine::IAL::I_GameTimer
 * @brief 游戏计时器的纯虚接口。
 * @details 抽象了 nclgl::GameTimer 或其他计时器实现。
 *
 * @fn Engine::IAL::I_GameTimer::~I_GameTimer
 * @brief 虚拟析构函数。
 *
 * @fn Engine::IAL::I_GameTimer::GetTimeDeltaSeconds
 * @brief 获取自上一帧以来所经过的时间，单位为秒。
 * @details
 * 适配器实现（如 B_GameTimer）负责将 nclgl::GameTimer::GetTimedMS()
 * 返回的毫秒值转换为秒（float）。
 * @return float 类型的增量时间（秒）。
 */

#pragma once

namespace Engine::IAL
{
    class I_GameTimer
    {
    public:
        virtual ~I_GameTimer() {}
        virtual float GetTimeDeltaSeconds() const = 0;
    };
    
}