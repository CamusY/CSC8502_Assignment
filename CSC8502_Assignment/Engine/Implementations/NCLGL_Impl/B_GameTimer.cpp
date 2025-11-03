#include "B_GameTimer.h"

// 在 .cpp 文件中包含 nclgl 的具体实现
#include "nclgl/GameTimer.h" 
#include <stdexcept> // 用于异常处理

namespace NCLGL_Impl {

    B_GameTimer::B_GameTimer(GameTimer* nclTimer) 
        : m_nclTimer(nclTimer) {
        if (!m_nclTimer) {
            throw std::runtime_error("B_GameTimer construction failed: the passed-in nclTimer pointer is null.");
        }
    }

    float B_GameTimer::GetTimeDeltaSeconds() const {
        // 直接将调用转发给被包装的 nclgl 实例
        return m_nclTimer->GetTimeDeltaSeconds(); 
    }

} // namespace NCLGL_Impl