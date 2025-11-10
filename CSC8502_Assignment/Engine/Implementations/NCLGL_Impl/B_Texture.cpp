/**
* @file B_Texture.cpp
 * @brief 轨道 B (NCLGL_Impl) 的纹理接口实现源文件。
 *
 * 本文件实现了 B_Texture 类。
 */
#include "B_Texture.h"

namespace {
    GLenum ResolveDefaultTarget(Engine::IAL::TextureType type) {
        using Engine::IAL::TextureType;
        switch (type) {
        case TextureType::Texture2D:
            return GL_TEXTURE_2D;
        case TextureType::CubeMap:
            return GL_TEXTURE_CUBE_MAP;
        case TextureType::Array2D:
            return GL_TEXTURE_2D_ARRAY;
        case TextureType::DepthStencil:
            return GL_TEXTURE_2D;
        case TextureType::External:
        default:
            return GL_TEXTURE_2D;
        }
    }
}

namespace NCLGL_Impl {

    B_Texture::B_Texture(unsigned int id, Engine::IAL::TextureType type, unsigned int overrideTarget)
        : m_id(id)
        , m_glTarget(overrideTarget)
        , m_type(type) {
        if (m_glTarget == 0) {
            m_glTarget = ResolveDefaultTarget(type);
        }
    }

    B_Texture::~B_Texture() {
        if (m_id != 0) {
            glDeleteTextures(1, &m_id);
            m_id = 0;
        }
    }

    unsigned int B_Texture::GetID() {
        return m_id;
    }

    Engine::IAL::TextureType B_Texture::GetType() const {
        return m_type;
    }

    void B_Texture::Bind(int slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(m_glTarget, m_id);
    }

}