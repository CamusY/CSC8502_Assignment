#pragma once

#include "IAL/I_Texture.h" // 包含我们要实现的 IAL 接口

// 1. (规范) B_Texture 必须调用 OpenGL API，因此需要包含 glad.h
//    请确保 glad.h 位于你的项目包含路径中
#include "glad/glad.h" //

namespace NCLGL_Impl {

    /**
     * @brief 轨道 B (nclgl) 纹理实现
     * @details
     * - 实现了 Engine::IAL::I_Texture 接口。
     * - nclgl 库没有纹理类，此适配器包装了由 B_Factory (通过 SOIL)
     * 加载生成的原始 GLuint 纹理ID。
     * - V13 规范 (4.2)：此类实例由 B_Factory 创建并包装在 std::shared_ptr 中，
     * 其析构函数负责释放 OpenGL 纹理资源。
     */
    class B_Texture : public Engine::IAL::I_Texture {
    public:
        /**
         * @brief 构造函数
         * @param textureID B_Factory 通过 SOIL_load_OGL_texture 等函数获取的 OpenGL ID
         * @param target OpenGL 纹理目标 (例如 GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP)
         */
        B_Texture(unsigned int textureID, unsigned int target);

        /**
         * @brief 析构函数
         * @details 负责调用 glDeleteTextures 释放 OpenGL 资源 (RAII)。
         */
        virtual ~B_Texture();

        // --- I_Texture 接口实现 ---

        /**
         * @brief 获取原生的 OpenGL 纹理 ID
         */
        virtual unsigned int GetID() const override;
        
        /**
         * @brief 绑定纹理到指定的纹理单元
         * @details 抽象了 glActiveTexture 和 glBindTexture
         * @param slot 纹理单元 (例如 0 对应 GL_TEXTURE0)
         */
        virtual void Bind(int slot = 0) const override;

    private:
        unsigned int m_textureID;   // 2. (规范) 封装的 OpenGL 纹理句柄
        unsigned int m_textureTarget; // 3. (规范) 纹理目标 (GL_TEXTURE_2D 等)
    };

} // namespace NCLGL_Impl