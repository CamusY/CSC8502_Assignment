#pragma once

#include <memory> // 用于 std::shared_ptr

// IAL 依赖 IAL (纯净)
// FBO 必须返回 I_Texture 接口
#include "IAL/I_Texture.h" 

// NFR-2: 接口直接依赖 nclgl 通用数学库
// (此文件不需要 nclgl 数学类型)

// NFR-1: 严禁 #include "nclgl/OGLRenderer.h" 或 "glad/glad.h"

namespace Engine::IAL {

    // (确保 I_Texture.h 已经定义)
    // class I_Texture;

    /**
     * @brief IAL 帧缓冲对象接口 (P-3, P-4)
     * @details
     * - 抽象了一个通用的帧缓冲对象 (FBO)。
     * - 这是满足课程高级要求 P-3 (后处理) 和 P-4 (阴影) 的核心。
     * - NFR-11.3: 关键风险！nclgl 库没有提供 FBO 类。
     * 因此 B_FrameBuffer (轨道 B) 实现必须是自研的，
     * 它将直接使用 OpenGL API (例如 glad.h 中包含的) 来创建、
     * 绑定和管理 FBO 及其附件。
     * - Bind: 抽象 glBindFramebuffer(GL_FRAMEBUFFER, id)。
     * - Unbind: 抽象 glBindFramebuffer(GL_FRAMEBUFFER, 0)。
     * - GetColorTexture: 返回 FBO 颜色附件的 I_Texture 接口。
     * - GetDepthTexture: 返回 FBO 深度附件的 I_Texture 接口（对于阴影贴图至关重要）。
     */
    class I_FrameBuffer {
    public:
        virtual ~I_FrameBuffer() = default; 

        I_FrameBuffer(const I_FrameBuffer&) = delete;
        I_FrameBuffer& operator=(const I_FrameBuffer&) = delete;
        I_FrameBuffer(I_FrameBuffer&&) = delete;
        I_FrameBuffer& operator=(I_FrameBuffer&&) = delete;

        // --- 纯虚接口函数 ---
        virtual void Bind() = 0;
        
        virtual void Unbind() = 0;
        
        virtual std::shared_ptr<I_Texture> GetColorTexture() const = 0;
        
        virtual std::shared_ptr<I_Texture> GetDepthTexture() const = 0;

    protected:
        I_FrameBuffer() = default;
    };
    
} // namespace Engine::IAL