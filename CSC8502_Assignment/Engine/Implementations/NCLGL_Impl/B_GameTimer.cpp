/**
* @file B_GameTimer.cpp
 * @brief 轨道 B (NCLGL_Impl) 的游戏计时器接口实现源文件。
 *
 * 本文件实现了 B_GameTimer 类。
 * 在 Day 2 阶段，GetTimeDeltaSeconds 返回 0 以确保可编译运行。
 * 构造函数中暂时不实例化 GameTimer，留待完整实现时添加。
 */
#include "B_GameTimer.h"
#include "nclgl/GameTimer.h"

namespace NCLGL_Impl {

    B_GameTimer::B_GameTimer() {
        m_timer = new ::GameTimer();
    }

    B_GameTimer::~B_GameTimer() {
        delete m_timer;
    }

    float B_GameTimer::GetTimeDeltaSeconds() const {
        return m_timer->GetTimeDeltaSeconds();
    }

}