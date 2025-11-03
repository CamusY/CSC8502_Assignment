#pragma once

#include <string>

// NFR-2: 接口直接依赖 nclgl 通用数学库
#include "nclgl/Vector3.h"
// (I_DebugUI.h 不需要 Matrix4, Vector2, Vector4, 故不引用)

namespace Engine::IAL {

    /**
     * @brief IAL 即时模式调试UI接口 (P-5)
     * @details
     * - 抽象了一个 IMGUI 系统（如 ImGui），用于满足 P-5 的“可调控参数”需求。
     * - NFR-11.3: 轨道 B (nclgl) 将提供 B_DebugUI_Null 空实现。
     * - NFR-11.3: 轨道 C (Custom) 必须提供一个功能完整的自研或集成的 IMGUI 实现。
     * - Init: 接收一个不透明的窗口句柄 (void*)，B 轨将传入 HWND，C 轨将传入 GLFWwindow*。
     * - SliderFloat3: NFR-2 规范，直接使用 nclgl::Vector3。
     */
    class I_DebugUI {
    public:
        virtual ~I_DebugUI() {}
        
        virtual void Init(void* windowHandle) = 0; 
        virtual void Shutdown() = 0;

        virtual void NewFrame() = 0;
        virtual void Render() = 0;

        virtual bool BeginWindow(const std::string& title) = 0;
        virtual void EndWindow() = 0;
        
        virtual void Text(const std::string& text) = 0;
        virtual bool Button(const std::string& label) = 0;
        virtual bool Checkbox(const std::string& label, bool* v) = 0;
        
        virtual bool SliderFloat(const std::string& label, float* v, float v_min, float v_max) = 0;
        virtual bool SliderFloat3(const std::string& label, Vector3* v, float v_min, float v_max) = 0;
        
        virtual bool ColorEdit3(const std::string& label, Vector3* v) = 0;
    };

} // namespace Engine::IAL