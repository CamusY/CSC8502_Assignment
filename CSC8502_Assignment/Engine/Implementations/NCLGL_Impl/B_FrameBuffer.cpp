#include "B_FrameBuffer.h"
#include "B_Texture.h" // 4. (规范) 包含 B_Texture 以便实例化
#include <stdexcept>
#include <iostream>

namespace NCLGL_Impl {

    B_FrameBuffer::B_FrameBuffer(int width, int height, bool createColorAttachment, bool createDepthAttachment)
        : m_fboID(0), m_width(width), m_height(height),
          m_defaultScreenWidth(width), m_defaultScreenHeight(height) { // 默认值

        // 5. (规范) NFR-11.3 自研 FBO 实现
        glGenFramebuffers(1, &m_fboID); //
        glBindFramebuffer(GL_FRAMEBUFFER, m_fboID); //

        if (createColorAttachment) {
            // 6. (规范) 为 P-3 (后处理) 创建颜色附件
            GLuint colorID;
            glGenTextures(1, &colorID); //
            glBindTexture(GL_TEXTURE_2D, colorID); //
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr); //
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorID, 0); //
            
            // 7. (规范) 将 GLuint 包装在 B_Texture 适配器中
            m_colorTexture = std::make_shared<B_Texture>(colorID, GL_TEXTURE_2D);
        }

        if (createDepthAttachment) {
            // 8. (规范) 为 P-4 (阴影) 创建深度附件
            GLuint depthID;
            glGenTextures(1, &depthID); //
            glBindTexture(GL_TEXTURE_2D, depthID); //
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr); //
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //
            // (用于阴影贴图)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE); //
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL); //
            
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthID, 0); //

            m_depthTexture = std::make_shared<B_Texture>(depthID, GL_TEXTURE_2D);
        }

        // 9. (规范) 检查 FBO 完整性
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { //
            glBindFramebuffer(GL_FRAMEBUFFER, 0); //
            throw std::runtime_error("B_FrameBuffer 构造失败：FBO 不完整。");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0); //
    }

    B_FrameBuffer::~B_FrameBuffer() {
        glDeleteFramebuffers(1, &m_fboID); //
        // m_colorTexture 和 m_depthTexture (shared_ptr) 会自动销毁
        // 它们的 B_Texture 析构函数会调用 glDeleteTextures
    }

    void B_FrameBuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fboID); //
        glViewport(0, 0, m_width, m_height); //
    }

    void B_FrameBuffer::Unbind() {
        // IAL 接口
        // 的 Unbind() 没有视口参数，这在实践中很危险。
        // 我们将实现一个更健壮的内部版本，
        // 并让 IAL Unbind() 恢复到已知的默认视口。
        BindDefaultScreen(m_defaultScreenWidth, m_defaultScreenHeight);
    }
    
    void B_FrameBuffer::BindDefaultScreen(int screenWidth, int screenHeight) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0); //
        glViewport(0, 0, screenWidth, screenHeight); //
        
        // 缓存主屏幕尺寸以供 IAL::Unbind() 使用
        m_defaultScreenWidth = screenWidth;
        m_defaultScreenHeight = screenHeight;
    }

    std::shared_ptr<Engine::IAL::I_Texture> B_FrameBuffer::GetColorTexture() const {
        return m_colorTexture;
    }

    std::shared_ptr<Engine::IAL::I_Texture> B_FrameBuffer::GetDepthTexture() const {
        return m_depthTexture;
    }

} // namespace NCLGL_Impl