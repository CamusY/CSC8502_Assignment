#pragma once

#include "IAL/I_FrameBuffer.h" // 1. (规范) 实现 IAL 接口
#include <memory>              // 用于 std::shared_ptr

// 2. (规范) NFR-11.3 
//    自研 FBO 需要直接访问 OpenGL API
#include "glad/glad.h" //

namespace NCLGL_Impl {

    // 3. (规范) B_FrameBuffer 创建并拥有 B_Texture 实例
    //    在 .h 中使用前向声明
    class B_Texture;

    /**
     * @brief 轨道 B (nclgl) 帧缓冲对象 (FBO) 实现
     * @details
     * - 实现了 Engine::IAL::I_FrameBuffer 接口。
     * - NFR-11.3: 这是一个自研的 FBO 实现，
     * 因为 nclgl 库没有提供此功能。
     * - 它直接调用 glad/glad.h 
     * 中的 OpenGL API 来创建和管理 FBO。
     * - 遵循 RAII，在析构时自动释放所有 GL 资源。
     */
    class B_FrameBuffer : public Engine::IAL::I_FrameBuffer {
    public:
        /**
         * @brief 构造函数
         * @param width FBO 宽度
         * @param height FBO 高度
         * @param createColorAttachment 是否创建颜色附件 (用于后处理)
         * @param createDepthAttachment 是否创建深度附件 (用于阴影贴图 P-4)
         */
        B_FrameBuffer(int width, int height, bool createColorAttachment, bool createDepthAttachment);
        
        /**
         * @brief 析构函数
         * @details 自动调用 glDeleteFramebuffers。
         * m_colorTexture 和 m_depthTexture 会通过 shared_ptr 自动销毁，
         * 触发 B_Texture 的析构函数并调用 glDeleteTextures。
         */
        virtual ~B_FrameBuffer();

        // --- I_FrameBuffer 接口实现 ---

        /**
         * @brief 绑定此 FBO 作为当前的渲染目标
         * @details 抽象了 glBindFramebuffer 和 glViewport
         */
        virtual void Bind() override;

        /**
         * @brief 解绑 FBO，恢复到默认的屏幕帧缓冲 (0)
         * @details 抽象了 glBindFramebuffer(GL_FRAMEBUFFER, 0)
         * @param screenWidth 恢复视口时使用的主屏幕宽度
         * @param screenHeight 恢复视口时使用的主屏幕高度
         */
        void BindDefaultScreen(int screenWidth, int screenHeight);

        /**
         * @brief (IAL 接口) 简单解绑 FBO
         */
        virtual void Unbind() override;

        /**
         * @brief 获取颜色附件的 I_Texture 接口
         */
        virtual std::shared_ptr<Engine::IAL::I_Texture> GetColorTexture() const override;
        
        /**
         * @brief 获取深度附件的 I_Texture 接口
         */
        virtual std::shared_ptr<Engine::IAL::I_Texture> GetDepthTexture() const override;

    private:
        GLuint m_fboID;
        int m_width;
        int m_height;

        // B_FrameBuffer 拥有它所创建的纹理
        std::shared_ptr<Engine::IAL::I_Texture> m_colorTexture;
        std::shared_ptr<Engine::IAL::I_Texture> m_depthTexture;

        // (可选) 用于 Unbind 时恢复视口
        // 注意：IAL 接口 
        // 的 Unbind 没有参数，这需要 B_WindowSystem 提供屏幕尺寸
        int m_defaultScreenWidth;
        int m_defaultScreenHeight;
    };

} // namespace NCLGL_Impl