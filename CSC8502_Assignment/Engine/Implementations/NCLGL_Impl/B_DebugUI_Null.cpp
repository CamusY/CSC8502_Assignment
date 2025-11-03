#include "B_DebugUI_Null.h"

// 2. (规范) NFR-11.3
//    所有函数均为空函数体，确保对 Demo 层透明且无害

namespace NCLGL_Impl {

    void B_DebugUI_Null::Init(void* windowHandle) { /* 空实现 */ }
    void B_DebugUI_Null::Shutdown() { /* 空实现 */ }
    void B_DebugUI_Null::NewFrame() { /* 空实现 */ }
    void B_DebugUI_Null::Render() { /* 空实现 */ }

    bool B_DebugUI_Null::BeginWindow(const std::string& title) {
        return false; // 返回 false，Demo 层（如果编写正确）会跳过窗口内容的渲染
    }
    void B_DebugUI_Null::EndWindow() { /* 空实现 */ }

    void B_DebugUI_Null::Text(const std::string& text) { /* 空实现 */ }
    
    bool B_DebugUI_Null::Button(const std::string& label) {
        return false; // 按钮永远不会被“按下”
    }
    
    bool B_DebugUI_Null::Checkbox(const std::string& label, bool* v) {
        return false; // 值永远不会被改变
    }

    bool B_DebugUI_Null::SliderFloat(const std::string& label, float* v, float v_min, float v_max) {
        return false; // 值永远不会被改变
    }

    bool B_DebugUI_Null::SliderFloat3(const std::string& label, Vector3* v, float v_min, float v_max) {
        return false; // 值永远不会被改变
    }

    bool B_DebugUI_Null::ColorEdit3(const std::string& label, Vector3* v) {
        return false; // 值永远不会被改变
    }

} // namespace NCLGL_Impl