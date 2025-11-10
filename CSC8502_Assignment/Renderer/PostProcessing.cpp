/**
 * @file PostProcessing.cpp
 * @brief 实现后期处理管线管理类。
 */
#include "PostProcessing.h"

#include "../Engine/IAL/I_FrameBuffer.h"
#include "../Engine/IAL/I_Texture.h"
#include "../Engine/IAL/I_Mesh.h"

#include <glad/glad.h>

#include <iostream>

PostProcessing::PostProcessing(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
                               int width,
                               int height)
    : m_factory(factory)
    , m_frameBuffer(nullptr)
    , m_fullscreenQuad(nullptr)
    , m_width(width)
    , m_height(height) {
    RecreateResources(width, height);
}

void PostProcessing::Resize(int width, int height) {
    if (width == m_width && height == m_height) {
        return;
    }
    RecreateResources(width, height);
}

void PostProcessing::BeginCapture() {
    if (!m_frameBuffer) {
        return;
    }
    //std::cerr << "[PostProcessing] Bind post-process FBO for scene capture" << std::endl;
    m_frameBuffer->Bind();
    glViewport(0, 0, m_width, m_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessing::EndCapture() {
    if (!m_frameBuffer) {
        return;
    }
    //std::cerr << "[PostProcessing] Unbind FBO after scene capture" << std::endl;
    m_frameBuffer->Unbind();
}

void PostProcessing::Present() {
    if (!m_frameBuffer) {
        return;
    }
    //std::cerr << "[PostProcessing] Present captured texture to default framebuffer" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_width, m_height);
    GLboolean depthEnabled = GL_FALSE;
    glGetBooleanv(GL_DEPTH_TEST, &depthEnabled);
    glDisable(GL_DEPTH_TEST);
    if (m_fullscreenQuad) {
        m_fullscreenQuad->Draw();
    }
    if (depthEnabled == GL_TRUE) {
        glEnable(GL_DEPTH_TEST);
    }
}

std::shared_ptr<Engine::IAL::I_Texture> PostProcessing::GetSceneTexture() const {
    if (!m_frameBuffer) {
        return nullptr;
    }
    return m_frameBuffer->GetColorTexture();
}

void PostProcessing::RecreateResources(int width, int height) {
    m_width = width;
    m_height = height;
    if (!m_factory) {
        m_frameBuffer.reset();
        m_fullscreenQuad.reset();
        return;
    }
    m_frameBuffer = m_factory->CreatePostProcessFBO(width, height);
    m_fullscreenQuad = m_factory->CreateQuad();
    std::cerr << "[PostProcessing] Recreate resources: " << width << "x" << height << std::endl;
}