/**
 * @file B_FrameBuffer.cpp
 * @brief 轨道 B (NCLGL_Impl) 的帧缓冲对象接口实现源文件。
 *
 * 本文件实现了 B_FrameBuffer 类。
 */
#include "B_FrameBuffer.h"
#include "B_Texture.h"

#include <glad/glad.h>

namespace NCLGL_Impl {

    B_FrameBuffer::B_FrameBuffer(int width, int height)
        : m_fboID(0), m_colorTexture(nullptr), m_depthTexture(nullptr) {
        glGenFramebuffers(1, &m_fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);

        unsigned int colorID = 0;
        glGenTextures(1, &colorID);
        glBindTexture(GL_TEXTURE_2D, colorID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorID, 0);
        m_colorTexture = std::make_shared<B_Texture>(colorID, GL_TEXTURE_2D);

        unsigned int depthID = 0;
        glGenTextures(1, &depthID);
        glBindTexture(GL_TEXTURE_2D, depthID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthID, 0);
        m_depthTexture = std::make_shared<B_Texture>(depthID, GL_TEXTURE_2D);

        const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, drawBuffers);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    B_FrameBuffer::~B_FrameBuffer() {
        GLint bound = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &bound);
        if (static_cast<unsigned int>(bound) == m_fboID) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        if (m_fboID != 0) {
            glDeleteFramebuffers(1, &m_fboID);
            m_fboID = 0;
        }
        m_colorTexture.reset();
        m_depthTexture.reset();
    }

    void B_FrameBuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
    }

    void B_FrameBuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    std::shared_ptr<Engine::IAL::I_Texture> B_FrameBuffer::GetColorTexture() {
        return m_colorTexture;
    }

    std::shared_ptr<Engine::IAL::I_Texture> B_FrameBuffer::GetDepthTexture() {
        return m_depthTexture;
    }

}