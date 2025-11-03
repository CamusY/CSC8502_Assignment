#pragma once

#include "IAL/I_DebugUI.h" // 1. (规范) 包含我们要实现的 IAL 接口

namespace NCLGL_Impl {

    /**
     * @brief 轨道 B (nclgl) 调试 UI 的空实现
     * @details
     * - 实现了 Engine::IAL::I_DebugUI 接口。
     * - NFR-11.3: nclgl 库缺乏 IMGUI 功能，
     * 因此轨道 B 必须提供一个功能性的“空对象”(Null Object)。
     * - 所有接口函数均为空函数体，或返回一个安全的默认值 (false)。
     * - 这确保了 Demo 层的 UI 调用在轨道 B 下是无害的。
     */
    class B_DebugUI_Null : public Engine::IAL::I_DebugUI {
    public:
        B_DebugUI_Null() = default;
        virtual ~B_DebugUI_Null() = default;

        // --- I_DebugUI 接口实现 (空实现) ---

        virtual void Init(void* windowHandle) override;
        virtual void Shutdown() override;

        virtual void NewFrame() override;
        virtual void Render() override;

        virtual bool BeginWindow(const std::string& title) override;
        virtual void EndWindow() override;

        virtual void Text(const std::string& text) override;
        virtual bool Button(const std::string& label) override;
        virtual bool Checkbox(const std::string& label, bool* v) override;

        virtual bool SliderFloat(const std::string& label, float* v, float v_min, float v_max) override;
        virtual bool SliderFloat3(const std::string& label, Vector3* v, float v_min, float v_max) override;

        virtual bool ColorEdit3(const std::string& label, Vector3* v) override;
    };

} // namespace NCLGL_Impl