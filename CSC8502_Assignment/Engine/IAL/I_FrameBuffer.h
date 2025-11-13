/**
* @file I_FrameBuffer.h
 * @brief 定义了帧缓冲对象（FBO）的抽象接口，用于离屏渲染。
 * @details
 * (NFR-11.3 关键风险) 
 * 这是 V13 架构中的一个关键自研组件。
 * `nclgl` 库（包括 `OGLRenderer.h`）
 * **确认缺乏** FBO 功能。
 *
 * 因此，此接口定义了实现以下功能所必需的抽象：
 * 1.  (P-3) 全屏后期处理效果：
 * 场景首先被渲染到 FBO 的颜色纹理上，
 * 然后该纹理被绘制到一个全屏四边形上。
 * 2.  (P-4) 阴影贴图：
 * 场景的深度信息从光源视角被渲染到 FBO 的
 * 深度纹理上。
 *
 * 轨道 B 的实现 `B_FrameBuffer` 
 * **必须**使用原生的 OpenGL API
 * (`glGenFramebuffers`, `glFramebufferTexture2D` 等) 
 * 来实现此接口的功能。
 *
 * @see I_ResourceFactory::CreatePostProcessFBO
 * @see I_ResourceFactory::CreateShadowFBO
 * @see I_Texture
 *
 * @class Engine::IAL::I_FrameBuffer
 * @brief 帧缓冲对象（FBO）的纯虚接口。
 *
 * @fn Engine::IAL::I_FrameBuffer::~I_FrameBuffer
 * @brief 虚拟析构函数。
 *
 * @fn Engine::IAL::I_FrameBuffer::Bind
 * @brief 绑定此 FBO 作为当前的渲染目标。
 * @details
 * 抽象了 `glBindFramebuffer(GL_FRAMEBUFFER, fboID)`。
 * 调用此函数后，所有的渲染命令将绘制到此 FBO 附加的纹理上，而不是屏幕。
 *
 * @fn Engine::IAL::I_FrameBuffer::Unbind
 * @brief 解绑 FBO，恢复到默认的窗口帧缓冲（屏幕）。
 * @details
 * 抽象了 `glBindFramebuffer(GL_FRAMEBUFFER, 0)`。
 *
 * @fn Engine::IAL::I_FrameBuffer::GetColorTexture
 * @brief 获取附加到此 FBO 的颜色纹理。
 * @details
 * (P-3) 
 * 后处理阶段将获取此纹理，并将其作为输入（例如 `uniform sampler2D`）
 * 绑定到着色器。
 * @return 一个 `std::shared_ptr<I_Texture>`，指向颜色附件。
 *
 * @fn Engine::IAL::I_FrameBuffer::GetDepthTexture
 * @brief 获取附加到此 FBO 的深度纹理。
 * @details
 * (P-4) 
 * 阴影渲染阶段将获取此纹理，并将其作为
 * `shadowMap` 采样器 
 * 传递给主场景着色器。
 * @return 一个 `std::shared_ptr<I_Texture>`，指向深度附件。
 */

#pragma once

#include <memory>

#include "IAL/I_Texture.h"

namespace Engine::IAL {
    class I_FrameBuffer {
    public:
        virtual ~I_FrameBuffer() {}
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
        virtual std::shared_ptr<I_Texture> GetColorTexture() = 0;
        virtual std::shared_ptr<I_Texture> GetDepthTexture() = 0;
    };

}
