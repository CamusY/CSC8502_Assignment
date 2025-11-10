/**
* @file B_Texture.cpp
 * @brief 轨道 B (NCLGL_Impl) 的纹理接口实现源文件。
 *
 * 本文件实现了 B_Texture 类。
 */
#include "B_Texture.h"

namespace NCLGL_Impl {

    B_Texture::B_Texture(unsigned int id, unsigned int target)
        : m_id(id), m_target(target) {
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

    void B_Texture::Bind(int slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(m_target, m_id);
    }

}