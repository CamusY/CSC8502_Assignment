/**
 * @file B_FrameBuffer.cpp
 * @brief 轨道 B (NCLGL_Impl) 的帧缓冲对象接口实现源文件。
 *
 * 本文件实现了 B_FrameBuffer 类。
 */
#include "B_FrameBuffer.h"
#include "B_Texture.h"

#include <glad/glad.h>
#include <iostream>

#include <string>

namespace {
    using AttachmentFormat = NCLGL_Impl::AttachmentFormat;

    std::string AttachmentFormatToString(AttachmentFormat format) {
        switch (format) {
        case AttachmentFormat::None: return "None";
        case AttachmentFormat::Color8: return "Color8";
        case AttachmentFormat::Color16F: return "Color16F";
        case AttachmentFormat::Depth24: return "Depth24";
        case AttachmentFormat::Depth32F: return "Depth32F";
        }
        return "Unknown";
    }
}

namespace {
    const char* TextureTypeToString(Engine::IAL::TextureType type) {
        using Engine::IAL::TextureType;
        switch (type) {
        case TextureType::Texture2D: return "Texture2D";
        case TextureType::CubeMap: return "CubeMap";
        case TextureType::Array2D: return "Array2D";
        case TextureType::DepthStencil: return "DepthStencil";
        case TextureType::External: return "External";
        default: return "Unknown";
        }
    }
}

namespace NCLGL_Impl {

    B_FrameBuffer::B_FrameBuffer(int width, int height, bool enableColorAttachment)
        : m_fboID(0),
          m_colorTexture(nullptr),
          m_depthTexture(nullptr),
          m_hasColorAttachment(enableColorAttachment),
          m_colorFormat(enableColorAttachment ? AttachmentFormat::Color8 : AttachmentFormat::None),
          m_depthFormat(AttachmentFormat::Depth24) {
        glGenFramebuffers(1, &m_fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);

        if (m_hasColorAttachment) {
            unsigned int colorID = 0;
            glGenTextures(1, &colorID);
            glBindTexture(GL_TEXTURE_2D, colorID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorID, 0);
            m_colorTexture = std::make_shared<B_Texture>(colorID, Engine::IAL::TextureType::Texture2D);
            std::cerr << "[B_FrameBuffer] Color attachment type: "
                      << TextureTypeToString(m_colorTexture->GetType()) << std::endl;
        }

        unsigned int depthID = 0;
        glGenTextures(1, &depthID);
        glBindTexture(GL_TEXTURE_2D, depthID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthID, 0);
        m_depthTexture = std::make_shared<B_Texture>(depthID, Engine::IAL::TextureType::DepthStencil);
        std::cerr << "[B_FrameBuffer] Depth attachment type: "
                  << TextureTypeToString(m_depthTexture->GetType()) << std::endl;

        if (m_hasColorAttachment) {
            const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, drawBuffers);
        }
        else {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }

        const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        const std::string layoutDesc = DescribeLayout();
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "[B_FrameBuffer] Incomplete GL_FRAMEBUFFER " << layoutDesc << " "
                      << width << "x" << height << ", status: 0x" << std::hex
                      << status << std::dec << std::endl;
        } else {
            std::cerr << "[B_FrameBuffer] GL_FRAMEBUFFER complete " << layoutDesc << " "
                      << width << "x" << height << ", status: 0x" << std::hex
                      << status << std::dec << std::endl;
        }
        
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

    AttachmentFormat B_FrameBuffer::GetColorFormat() const {
        return m_colorFormat;
    }

    AttachmentFormat B_FrameBuffer::GetDepthFormat() const {
        return m_depthFormat;
    }

    std::string B_FrameBuffer::DescribeLayout() const {
        const bool hasColor = m_hasColorAttachment && m_colorFormat != AttachmentFormat::None;
        const std::string depthStr = AttachmentFormatToString(m_depthFormat);

        if (!hasColor) {
            if (m_depthFormat == AttachmentFormat::None) {
                return "Empty (None)";
            }
            return "Depth-only (" + depthStr + ")";
        }

        std::string description = "Color+Depth (" + AttachmentFormatToString(m_colorFormat);
        if (m_depthFormat != AttachmentFormat::None) {
            description += "/" + depthStr;
        }
        description += ")";
        return description;
    }

}