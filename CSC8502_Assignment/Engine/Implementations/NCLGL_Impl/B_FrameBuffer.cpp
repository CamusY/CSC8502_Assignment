/**
 * @file B_FrameBuffer.cpp
 * @brief 轨道 B (NCLGL_Impl) 的帧缓冲对象接口实现源文件。
 *
 * 本文件实现了 B_FrameBuffer 类。
 */
#include "B_FrameBuffer.h"
#include "B_Texture.h"

#include <iostream>

#include <string>

namespace NCLGL_Impl {
    B_FrameBuffer::B_FrameBuffer(int width, int height, bool enableColorAttachment) :
        m_fboID(0),
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
                << TextureTypeToString(m_colorTexture->GetType()) << "\n";
            GLint minFilter = 0;
            GLint magFilter = 0;
            GLint wrapS = 0;
            GLint wrapT = 0;
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapS);
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapT);
            std::cerr << "[B_FrameBuffer] FBO " << m_fboID << " color attachment "
                << width << "x" << height << " sampler: MIN=" << FilterToString(minFilter)
                << ", MAG=" << FilterToString(magFilter) << ", WRAP_S=" << WrapToString(wrapS)
                << ", WRAP_T=" << WrapToString(wrapT) << "\n";
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
            << TextureTypeToString(m_depthTexture->GetType()) << "\n";
        GLint depthMinFilter = 0;
        GLint depthMagFilter = 0;
        GLint depthWrapS = 0;
        GLint depthWrapT = 0;
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &depthMinFilter);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &depthMagFilter);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &depthWrapS);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &depthWrapT);
        std::cerr << "[B_FrameBuffer] FBO " << m_fboID << " depth attachment "
            << width << "x" << height << " sampler: MIN=" << FilterToString(depthMinFilter)
            << ", MAG=" << FilterToString(depthMagFilter) << ", WRAP_S=" << WrapToString(depthWrapS)
            << ", WRAP_T=" << WrapToString(depthWrapT) << "\n";

        if (m_hasColorAttachment) {
            const GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
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
                << status << std::dec << "\n";
        }
        else {
            std::cerr << "[B_FrameBuffer] GL_FRAMEBUFFER complete " << layoutDesc << " "
                << width << "x" << height << ", status: 0x" << std::hex
                << status << std::dec << "\n";
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

    std::string AttachmentFormatToString(AttachmentFormat format) {
        switch (format) {
        case AttachmentFormat::None:
            return "None";
        case AttachmentFormat::Color8:
            return "Color8";
        case AttachmentFormat::Color16F:
            return "Color16F";
        case AttachmentFormat::Depth24:
            return "Depth24";
        case AttachmentFormat::Depth32F:
            return "Depth32F";
        }
        return "Unknown";
    }

    std::string FilterToString(GLint value) {
        switch (value) {
        case GL_NEAREST:
            return "GL_NEAREST";
        case GL_LINEAR:
            return "GL_LINEAR";
        case GL_NEAREST_MIPMAP_NEAREST:
            return "GL_NEAREST_MIPMAP_NEAREST";
        case GL_LINEAR_MIPMAP_NEAREST:
            return "GL_LINEAR_MIPMAP_NEAREST";
        case GL_NEAREST_MIPMAP_LINEAR:
            return "GL_NEAREST_MIPMAP_LINEAR";
        case GL_LINEAR_MIPMAP_LINEAR:
            return "GL_LINEAR_MIPMAP_LINEAR";
        default:
            return "Unknown";
        }
    }

    std::string WrapToString(GLint value) {
        switch (value) {
        case GL_CLAMP_TO_EDGE:
            return "GL_CLAMP_TO_EDGE";
        case GL_CLAMP_TO_BORDER:
            return "GL_CLAMP_TO_BORDER";
        case GL_MIRRORED_REPEAT:
            return "GL_MIRRORED_REPEAT";
        case GL_REPEAT:
            return "GL_REPEAT";
        case GL_MIRROR_CLAMP_TO_EDGE:
            return "GL_MIRROR_CLAMP_TO_EDGE";
        default:
            return "Unknown";
        }
    }

    const char* TextureTypeToString(Engine::IAL::TextureType type) {
        using Engine::IAL::TextureType;
        switch (type) {
        case TextureType::Texture2D:
            return "Texture2D";
        case TextureType::CubeMap:
            return "CubeMap";
        case TextureType::Array2D:
            return "Array2D";
        case TextureType::DepthStencil:
            return "DepthStencil";
        case TextureType::External:
            return "External";
        default:
            return "Unknown";
        }

    }
}