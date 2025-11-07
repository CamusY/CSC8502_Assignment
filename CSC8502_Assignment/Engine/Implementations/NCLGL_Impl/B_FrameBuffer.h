/**
* @file B_FrameBuffer.h
 * @brief 轨道 B (NCLGL_Impl) 的帧缓冲对象接口实现。
 *
 * 本文件定义了 B_FrameBuffer 类，它是 Engine::IAL::I_FrameBuffer 接口在轨道 B 下的具体实现。
 *
 * 重要说明 (NFR-11.3):
 * 由于 nclgl 库本身未提供 FrameBuffer 的封装类，B_FrameBuffer 必须直接使用原生的 OpenGL API
 * (如 glGenFramebuffers, glBindFramebuffer 等) 来实现所有功能。
 * 它是轨道 B 中少有的非适配器、完全自研的类之一。
 *
 * B_FrameBuffer 类 (NCLGL_Impl::B_FrameBuffer):
 * 继承自 Engine::IAL::I_FrameBuffer 纯虚接口。
 *
 * 构造函数 B_FrameBuffer(int width, int height):
 * 接收期望的 FBO 尺寸。在完整实现中，它负责创建 OpenGL FBO 句柄、纹理附件和深度附件。
 *
 * 析构函数 ~B_FrameBuffer():
 * 负责销毁 OpenGL FBO 资源及关联的纹理附件。
 *
 * 成员函数 Bind() / Unbind():
 * 切换渲染目标到此 FBO 或返回默认帧缓冲。
 *
 * 成员函数 GetColorTexture() / GetDepthTexture():
 * 返回作为附件的纹理对象，供后续渲染管线使用（例如作为后处理的输入）。
 *
 * (Day 2 阶段注：以下成员变量仅作为占位符，尚未被初始化或使用)
 * 成员变量 m_fboID: OpenGL 帧缓冲对象句柄。
 * 成员变量 m_colorTexture: 包装了颜色附件 OpenGL 句柄的纹理对象。
 * 成员变量 m_depthTexture: 包装了深度附件 OpenGL 句柄的纹理对象。
 */
#pragma once
#include "IAL/I_FrameBuffer.h"

namespace NCLGL_Impl {

    class B_FrameBuffer : public Engine::IAL::I_FrameBuffer {
    public:
        B_FrameBuffer(int width, int height);
        ~B_FrameBuffer() override;

        void Bind() override;
        void Unbind() override;

        std::shared_ptr<Engine::IAL::I_Texture> GetColorTexture() override;
        std::shared_ptr<Engine::IAL::I_Texture> GetDepthTexture() override;

    private:
        unsigned int m_fboID;
        std::shared_ptr<Engine::IAL::I_Texture> m_colorTexture;
        std::shared_ptr<Engine::IAL::I_Texture> m_depthTexture;
    };

}