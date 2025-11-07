/**
* @file B_FrameBuffer.cpp
 * @brief 轨道 B (NCLGL_Impl) 的帧缓冲对象接口实现源文件。
 *
 * 本文件实现了 B_FrameBuffer 类。
 * 在 Day 2 阶段，它是一个空壳实现。
 * 构造函数未执行任何 OpenGL 创建操作，Getter 函数返回空指针，确保能通过编译链接即可。
 */
#include "B_FrameBuffer.h"

namespace NCLGL_Impl {

    B_FrameBuffer::B_FrameBuffer(int width, int height)
        : m_fboID(0), m_colorTexture(nullptr), m_depthTexture(nullptr) {
    }

    B_FrameBuffer::~B_FrameBuffer() {
    }

    void B_FrameBuffer::Bind() {
    }

    void B_FrameBuffer::Unbind() {
    }

    std::shared_ptr<Engine::IAL::I_Texture> B_FrameBuffer::GetColorTexture() {
        return m_colorTexture;
    }

    std::shared_ptr<Engine::IAL::I_Texture> B_FrameBuffer::GetDepthTexture() {
        return m_depthTexture;
    }

}