#pragma once

// NFR-2: 接口直接依赖 nclgl 通用数学库
// (此文件不需要 nclgl 数学类型)

// NFR-1: 严禁 #include "nclgl/OGLRenderer.h" 或 "SOIL/SOIL.h"

namespace Engine::IAL {

    /**
     * @brief IAL 纹理接口 (P-0, P-8)
     * @details
     * - 抽象了 nclgl 库通过 SOIL 加载并生成的 OpenGL 纹理ID (GLuint)。
     * - nclgl 库本身没有纹理类，B_Texture (轨道 B) 将包装一个 GLuint。
     * - GetID: 允许渲染器获取原生的 OpenGL 句柄。
     * - Bind: 抽象了 glBindTexture(target, id) 和 glActiveTexture(GL_TEXTURE0 + slot)。
     */
    class I_Texture {
    public:
        virtual ~I_Texture() {}
        
        virtual unsigned int GetID() const = 0;
        
        virtual void Bind(int slot = 0) const = 0;
    };
    
} // namespace Engine::IAL