/**
* @file B_GameTimer.h
 * @brief 轨道 B (NCLGL_Impl) 的游戏计时器接口实现。
 *
 * 本文件定义了 B_GameTimer 类，它是 Engine::IAL::I_GameTimer 接口的具体实现。
 * 它负责包装 nclgl::GameTimer，为引擎提供统一的时间增量查询接口。
 *
 * B_GameTimer 类 (NCLGL_Impl::B_GameTimer):
 * 继承自 Engine::IAL::I_GameTimer 纯虚接口。
 *
 * 构造函数 B_GameTimer():
 * 在内部实例化一个 nclgl::GameTimer 对象。
 *
 * 析构函数 ~B_GameTimer():
 * 释放内部的 nclgl::GameTimer 对象。
 *
 * 成员函数 GetTimeDeltaSeconds():
 * 实现 I_GameTimer 接口。
 * 返回自上一帧以来经过的时间（以秒为单位）。
 *
 * 成员变量 m_timer:
 * 指向原生的 nclgl::GameTimer 对象。
 */
#pragma once
#include "IAL/I_GameTimer.h"

class GameTimer;

namespace NCLGL_Impl {

    class B_GameTimer : public Engine::IAL::I_GameTimer {
    public:
        B_GameTimer();
        ~B_GameTimer() override;

        float GetTimeDeltaSeconds() const override;

    private:
        ::GameTimer* m_timer;
    };

}