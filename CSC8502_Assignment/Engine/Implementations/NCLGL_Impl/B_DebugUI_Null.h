/**
* @file B_DebugUI_Null.h
 * @brief 轨道 B (NCLGL_Impl) 的调试 UI 空接口实现。
 *
 * 本文件定义了 B_DebugUI_Null 类，它是 Engine::IAL::I_DebugUI 接口的一个“空对象” (Null Object) 实现。
 * 由于 nclgl 库原生不提供即时模式 GUI (IMGUI) 功能，且轨道 B 的重点在于核心图形功能的复现，
 * 因此该类用于满足 I_DebugUI 的接口契约，但所有操作均不执行任何实际逻辑。
 *
 * B_DebugUI_Null 类 (NCLGL_Impl::B_DebugUI_Null):
 * 继承自 Engine::IAL::I_DebugUI 纯虚接口。
 *
 * 成员函数 Init, NewFrame, Render, Shutdown, EndWindow, Text:
 * 空函数体，不执行任何操作。
 *
 * 成员函数 BeginWindow, SliderFloat, SliderFloat3, Checkbox, Button, ColorEdit3:
 * 返回 false 或默认值，确保调用方的逻辑能继续执行而不崩溃。
 */
#pragma once
#include "IAL/I_DebugUI.h"

namespace NCLGL_Impl {

    class B_DebugUI_Null : public Engine::IAL::I_DebugUI {
    public:
        B_DebugUI_Null();
        ~B_DebugUI_Null() override;

        void Init(void* windowHandle) override;
        void NewFrame() override;
        void Render() override;
        void Shutdown() override;

        bool BeginWindow(const std::string& title) override;
        void EndWindow() override;
        bool SliderFloat(const std::string& label, float* v, float v_min, float v_max) override;
        bool SliderFloat3(const std::string& label, Vector3* v, float v_min, float v_max) override;
        bool Checkbox(const std::string& label, bool* v) override;
        bool Button(const std::string& label) override;
        void Text(const std::string& text) override;
        bool ColorEdit3(const std::string& label, Vector3* v) override;
    };

}