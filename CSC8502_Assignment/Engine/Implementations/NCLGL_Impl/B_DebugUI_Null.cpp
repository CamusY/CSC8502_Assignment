/**
* @file B_DebugUI_Null.cpp
 * @brief 轨道 B (NCLGL_Impl) 的调试 UI 空接口实现源文件。
 *
 * 本文件实现了 B_DebugUI_Null 类。
 * 所有函数均为不执行任何操作的空实现，用于在不具备 GUI 支持的环境中安全地从核心引擎代码中调用 UI 接口。
 */
#include "B_DebugUI_Null.h"

namespace NCLGL_Impl {

    B_DebugUI_Null::B_DebugUI_Null() {
    }

    B_DebugUI_Null::~B_DebugUI_Null() {
    }

    void B_DebugUI_Null::Init(void* windowHandle) {
    }

    void B_DebugUI_Null::NewFrame() {
    }

    void B_DebugUI_Null::Render() {
    }

    void B_DebugUI_Null::Shutdown() {
    }

    bool B_DebugUI_Null::BeginWindow(const std::string& title) {
        return false;
    }

    void B_DebugUI_Null::EndWindow() {
    }

    bool B_DebugUI_Null::SliderFloat(const std::string& label, float* v, float v_min, float v_max) {
        return false;
    }

    bool B_DebugUI_Null::SliderFloat3(const std::string& label, Vector3* v, float v_min, float v_max) {
        return false;
    }

    bool B_DebugUI_Null::Checkbox(const std::string& label, bool* v) {
        return false;
    }

    bool B_DebugUI_Null::Button(const std::string& label) {
        return false;
    }

    void B_DebugUI_Null::Text(const std::string& text) {
    }

    bool B_DebugUI_Null::ColorEdit3(const std::string& label, Vector3* v) {
        return false;
    }

}