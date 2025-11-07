/**
* @file B_Texture.cpp
 * @brief 轨道 B (NCLGL_Impl) 的纹理接口实现源文件。
 *
 * 本文件实现了 B_Texture 类。
 * 在 Day 2 阶段，均为基础的空壳实现。
 * 注意：虽然是空壳，但为了能在 Day 2 编译通过，GetID 返回了 0。
 */
#include "B_Texture.h"

namespace NCLGL_Impl {

    B_Texture::B_Texture(unsigned int id) : m_id(id) {
    }

    B_Texture::~B_Texture() {
    }

    unsigned int B_Texture::GetID() {
        return m_id;
    }

    void B_Texture::Bind(int slot) {
    }

}