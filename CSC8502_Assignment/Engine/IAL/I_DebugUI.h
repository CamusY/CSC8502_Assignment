/**
 * @file I_DebugUI.h
 * @brief 定义了即时模式调试 GUI（IMGUI）的抽象接口。
 * @details
 * 该文件的设计目的是为核心应用层（CSC8502_Demo）提供一个统一的、与后端无关的
 * 方式来创建和渲染调试窗口，以满足 P-5（可通过 UI 调控的参数）的需求。
 *
 * (NFR-11.3 关键风险) nclgl 库确认缺乏 GUI 系统。
 * 因此，此接口在 V13 架构中的角色是双重的：
 * 1.  **轨道 B (NCLGL_Impl)**：将由 `B_DebugUI_Null` 实现。
 * 这是一个“空实现”，所有函数（Init, SliderFloat 等）均为空函数体。
 * 这使得 Demo 层的 UI 调用在轨道 B 下可以安全地编译和运行，但无任何效果。
 * 2.  **轨道 C (Custom_Impl)**：将由一个功能完整的自研或集成的 IMGUI
 * 系统（如 `C_DebugUI`）来实现。
 *
 * (NFR-2) 规范要求：本文件被授权且必须包含 nclgl 数学库（如 Vector3.h），
 * 以便在函数签名中直接使用 nclgl 的具体数学类型（如 SliderFloat3, ColorEdit3）。
 *
 * @class Engine::IAL::I_DebugUI
 * @brief 即时模式调试 GUI 的纯虚接口。
 * @details
 * 抽象了 IMGUI 系统的生命周期和控件绘制。
 * 实例由 I_ResourceFactory 或 main.cpp 中的依赖注入逻辑创建。
 *
 * @fn Engine::IAL::I_DebugUI::~I_DebugUI
 * @brief 虚拟析构函数。
 *
 * @fn Engine::IAL::I_DebugUI::Init(void* windowHandle)
 * @brief 初始化 IMGUI 系统并将其绑定到窗口上下文。
 * @details
 * (NFR-9) `void*` 句柄是一个有意的类型擦除，以避免在 IAL 层
 * `#include` 任何具体窗口的头文件（如 nclgl/Window.h 或 GLFW/glfw3.h）。
 * 这个句柄将由 `I_WindowSystem::GetHandle()` 提供。
 * @param windowHandle 指向原生窗口句柄（例如 HWND 或 GLFWwindow*）的指针。
 *
 * @fn Engine::IAL::I_DebugUI::NewFrame
 * @brief 通知 IMGUI 系统新的一帧已经开始。
 * @details 必须在所有 IMGUI 控件函数（如 BeginWindow, SliderFloat）之前调用。
 *
 * @fn Engine::IAL::I_DebugUI::Render
 * @brief 编译所有 IMGUI 控件的绘制列表并将其渲染到屏幕。
 * @details 应该在主场景渲染之后、交换缓冲区之前调用。
 *
 * @fn Engine::IAL::I_DebugUI::Shutdown
 * @brief 清理和关闭 IMGUI 系统。
 *
 * @fn Engine::IAL::I_DebugUI::BeginWindow(const std::string& title)
 * @brief (P-5) 开始一个新的 IMGUI 窗口容器。
 * @param title 窗口标题栏上显示的文本。
 * @return 如果窗口未折叠，则为 true，表示应渲染窗口内的控件。
 *
 * @fn Engine::IAL::I_DebugUI::EndWindow
 * @brief (P-5) 结束当前的 IMGUI 窗口容器。
 *
 * @fn Engine::IAL::I_DebugUI::SliderFloat
 * @brief (P-5) 绘制一个单浮点数滑块控件。
 * @param label 控件旁显示的标签。
 * @param v 指向要修改的 float 变量的指针。
 * @param v_min 滑块的最小值。
 * @param v_max 滑块的最大值。
 * @return 如果控件被用户交互，则为 true。
 *
 * @fn Engine::IAL::I_DebugUI::SliderFloat3
 * @brief (P-5) 绘制一个三浮点数（Vector3）滑块控件。
 * @details (NFR-2) 此函数签名必须直接使用 nclgl::Vector3。
 * @param label 控件旁显示的标签。
 * @param v 指向要修改的 nclgl::Vector3 变量的指针。
 * @param v_min 滑块的最小值。
 * @param v_max 滑块的最大值。
 * @return 如果控件被用户交互，则为 true。
 *
 * @fn Engine::IAL::I_DebugUI::Checkbox
 * @brief (P-5) 绘制一个复选框控件。
 * @param label 控件旁显示的标签。
 * @param v 指向要修改的 bool 变量的指针。
 * @return 如果控件被用户交互，则为 true。
 *
 * @fn Engine::IAL::I_DebugUI::Button
 * @brief (P-5) 绘制一个可点击的按钮。
 * @param label 按钮上显示的文本。
 * @return 如果按钮在本帧被点击，则为 true。
 *
 * @fn Engine::IAL::I_DebugUI::Text
 * @brief (P-5) 绘制静态文本。
 * @param text 要显示的字符串。
 *
 * @fn Engine::IAL::I_DebugUI::ColorEdit3
 * @brief (P-5) 绘制一个 RGB 颜色拾取器控件。
 * @details (NFR-2) 此函数签名必须直接使用 nclgl::Vector3，其中 (x, y, z) 对应 (R, G, B)。
 * @param label 控件旁显示的标签。
 * @param v 指向要修改的 nclgl::Vector3 颜色变量的指针。
 * @return 如果控件被用户交互，则为 true。
 */

#pragma once

#include <string>

// (NFR-2) 接口直接依赖 nclgl 通用数学库
#include "nclgl/Vector3.h"

namespace Engine::IAL {
    class I_DebugUI {
    public:
        virtual ~I_DebugUI() {}

        virtual void Init(void* windowHandle) = 0;
        virtual void NewFrame() = 0;
        virtual void Render() = 0;
        virtual void Shutdown() = 0;

        virtual bool BeginWindow(const std::string& title) = 0;
        virtual void EndWindow() = 0;

        virtual bool SliderFloat(const std::string& label, float* v, float v_min, float v_max) = 0;
        virtual bool SliderFloat3(const std::string& label, Vector3* v, float v_min, float v_max) = 0;

        virtual bool Checkbox(const std::string& label, bool* v) = 0;
        virtual bool Button(const std::string& label) = 0;
        virtual void Text(const std::string& text) = 0;

        virtual bool ColorEdit3(const std::string& label, Vector3* v) = 0;
    };

}
