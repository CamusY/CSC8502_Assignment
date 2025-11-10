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
* 构造函数 B_FrameBuffer(int width, int height, bool enableColorAttachment):
* 根据 enableColorAttachment 创建不同的资源布局：
*   - 当 enableColorAttachment 为 true 时，FBO 同时拥有 RGBA 颜色附件与深度附件，可用于后处理。
*   - 当 enableColorAttachment 为 false 时，仅创建深度附件，同时通过 glDrawBuffer/glReadBuffer 设置
*     GL_NONE，以生成阴影贴图使用的深度专用 FBO。
*
* 析构函数 ~B_FrameBuffer():
* 负责解绑并销毁 OpenGL FBO 资源，同时释放纹理附件。
*
* 成员函数 Bind() / Unbind():
* 切换渲染目标到此 FBO 或返回默认帧缓冲。
*
* 成员函数 GetColorTexture() / GetDepthTexture():
* 返回作为附件的纹理对象；当 FBO 为深度专用模式时，GetColorTexture() 将返回空指针。
*
* 成员变量 m_fboID: OpenGL 帧缓冲对象句柄。
* 成员变量 m_colorTexture: 包装了颜色附件 OpenGL 句柄的纹理对象（仅在启用颜色附件时存在）。
* 成员变量 m_depthTexture: 包装了深度附件 OpenGL 句柄的纹理对象。
* 成员变量 m_hasColorAttachment: 标记当前 FBO 是否拥有颜色附件。
*/
#pragma once
#include "IAL/I_FrameBuffer.h"

#include <string>

namespace NCLGL_Impl {
    enum class AttachmentFormat {
        None,
        Color8,
        Color16F,
        Depth24,
        Depth32F
    };
    
    class B_FrameBuffer : public Engine::IAL::I_FrameBuffer {
    public:
        B_FrameBuffer(int width, int height, bool enableColorAttachment);
        ~B_FrameBuffer() override;

        void Bind() override;
        void Unbind() override;

        std::shared_ptr<Engine::IAL::I_Texture> GetColorTexture() override;
        std::shared_ptr<Engine::IAL::I_Texture> GetDepthTexture() override;
        AttachmentFormat GetColorFormat() const;
        AttachmentFormat GetDepthFormat() const;

    private:
        std::string DescribeLayout() const;
        unsigned int m_fboID;
        std::shared_ptr<Engine::IAL::I_Texture> m_colorTexture;
        std::shared_ptr<Engine::IAL::I_Texture> m_depthTexture;
        bool m_hasColorAttachment;
        AttachmentFormat m_colorFormat;
        AttachmentFormat m_depthFormat;        
    };

}