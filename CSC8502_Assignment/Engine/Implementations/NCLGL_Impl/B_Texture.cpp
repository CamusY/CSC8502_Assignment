#include "B_Texture.h"
#include <stdexcept>

namespace NCLGL_Impl {

    B_Texture::B_Texture(unsigned int textureID, unsigned int target)
        : m_textureID(textureID), m_textureTarget(target) {
        if (m_textureID == 0) {
            // 注意：SOIL 加载失败会返回 0
            throw std::runtime_error("B_Texture 构造失败：传入的 textureID 为 0。");
        }
    }

    B_Texture::~B_Texture() {
        // 4. (规范) 遵循 RAII，当 shared_ptr 销毁此对象时，自动释放 GL 资源
        glDeleteTextures(1, &m_textureID); //
    }

    unsigned int B_Texture::GetID() const {
        return m_textureID;
    }

    void B_Texture::Bind(int slot) const {
        // 5. (规范) 抽象 IAL::I_Texture::Bind
        glActiveTexture(GL_TEXTURE0 + slot); //
        glBindTexture(m_textureTarget, m_textureID); //
    }

} // namespace NCLGL_Impl