/**
 * @file PostProcessing.cpp
 * @brief 实现后期处理管线管理类。
 */
#include "PostProcessing.h"

#include "../Engine/IAL/I_FrameBuffer.h"
#include "../Engine/IAL/I_Texture.h"
#include "../Engine/IAL/I_Mesh.h"
#include "../Engine/IAL/I_Shader.h"

#include <glad/glad.h>

#include <algorithm>
#include <iostream>

PostProcessing::PostProcessing(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
                               int width,
                               int height) :
    m_factory(factory),
    m_frameBuffer(nullptr),
    m_brightFrameBuffer(nullptr),
    m_pingPongBuffers{nullptr, nullptr},
    m_fullscreenQuad(nullptr),
    m_passthroughShader(nullptr),
    m_transitionShader(nullptr),
    m_brightShader(nullptr),
    m_blurShader(nullptr),
    m_width(width),
    m_height(height),
    m_bloomDirty(true),
    m_exposure(1.0f),
    m_cachedBloomTexture(nullptr) {
    if (m_factory) {
        m_passthroughShader = m_factory->CreateShader("Shared/postprocess.vert", "Shared/postprocess.frag");
        m_transitionShader = m_factory->CreateShader("Shared/postprocess.vert", "Shared/post_transition.frag");
        m_brightShader = m_factory->CreateShader("Shared/postprocess.vert", "Shared/post_bloom_extract.frag");
        m_blurShader = m_factory->CreateShader("Shared/postprocess.vert", "Shared/post_bloom_blur.frag");
    }
    RecreateResources(width, height);
}

void PostProcessing::Resize(int width, int height) {
    if (width == m_width && height == m_height) {
        return;
    }
    RecreateResources(width, height);
    m_bloomDirty = true;
}

void PostProcessing::BeginCapture() {
    if (!m_frameBuffer) {
        return;
    }
    m_frameBuffer->Bind();
    glViewport(0, 0, m_width, m_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_bloomDirty = true;
}

void PostProcessing::EndCapture() {
    if (!m_frameBuffer) {
        return;
    }
    m_frameBuffer->Unbind();
}

void PostProcessing::PresentToViewport(OutputMode mode,
                                       bool transitionEnabled,
                                       float timer,
                                       int viewportX,
                                       int viewportY,
                                       int viewportWidth,
                                       int viewportHeight) {
    if (!m_frameBuffer || !m_fullscreenQuad) {
        return;
    }
    auto texture = m_frameBuffer->GetColorTexture();
    if (!texture) {
        return;
    }

    ProcessBloom();
    auto bloomTexture = m_cachedBloomTexture;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
    GLboolean depthEnabled = GL_FALSE;
    glGetBooleanv(GL_DEPTH_TEST, &depthEnabled);
    glDisable(GL_DEPTH_TEST);

    std::shared_ptr<Engine::IAL::I_Shader> shader = transitionEnabled && m_transitionShader
        ? m_transitionShader
        : m_passthroughShader;
    if (shader) {
        shader->Bind();
        texture->Bind(0);
        shader->SetUniform("uScene", 0);
        if (bloomTexture) {
            bloomTexture->Bind(1);
            shader->SetUniform("uBloom", 1);
        }
        const int displayMode = mode == OutputMode::BloomOnly ? 1 : (mode == OutputMode::RawScene ? 2 : 0);
        shader->SetUniform("uDisplayMode", displayMode);
        shader->SetUniform("uExposure", m_exposure);
        if (transitionEnabled && shader == m_transitionShader) {
            shader->SetUniform("uTimer", std::clamp(timer, 0.0f, 1.0f));
        }
        m_fullscreenQuad->Draw();
        shader->Unbind();
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

std::shared_ptr<Engine::IAL::I_Texture> PostProcessing::GetBloomTexture() const {
    return m_cachedBloomTexture;
}

void PostProcessing::SetExposure(float exposure) {
    m_exposure = std::max(0.01f, exposure);
}

void PostProcessing::RecreateResources(int width, int height) {
    m_width = width;
    m_height = height;
    m_cachedBloomTexture.reset();
    if (!m_factory) {
        m_frameBuffer.reset();
        m_brightFrameBuffer.reset();
        m_pingPongBuffers[0].reset();
        m_pingPongBuffers[1].reset();
        m_fullscreenQuad.reset();
        return;
    }
    m_frameBuffer = m_factory->CreatePostProcessFBO(width, height);
    m_brightFrameBuffer = m_factory->CreatePostProcessFBO(width, height);
    m_pingPongBuffers[0] = m_factory->CreatePostProcessFBO(width, height);
    m_pingPongBuffers[1] = m_factory->CreatePostProcessFBO(width, height);
    m_fullscreenQuad = m_factory->CreateQuad();
    std::cerr << "[PostProcessing] Recreate resources: " << width << "x" << height << "\n";
    m_bloomDirty = true;
}

void PostProcessing::ProcessBloom() {
    if (!m_bloomDirty) {
        return;
    }
    m_bloomDirty = false;
    m_cachedBloomTexture.reset();
    if (!m_frameBuffer || !m_brightFrameBuffer || !m_brightShader || !m_blurShader || !m_fullscreenQuad) {
        return;
    }

    auto sceneTexture = m_frameBuffer->GetColorTexture();
    if (!sceneTexture) {
        return;
    }

    GLboolean depthWasEnabled = GL_FALSE;
    glGetBooleanv(GL_DEPTH_TEST, &depthWasEnabled);
    if (depthWasEnabled == GL_TRUE) {
        glDisable(GL_DEPTH_TEST);
    }
    
    m_brightFrameBuffer->Bind();
    glViewport(0, 0, m_width, m_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_brightShader->Bind();
    sceneTexture->Bind(0);
    m_brightShader->SetUniform("uScene", 0);
    m_brightShader->SetUniform("uThreshold", 1.0f);
    m_fullscreenQuad->Draw();
    m_brightShader->Unbind();
    m_brightFrameBuffer->Unbind();

    bool horizontal = true;
    bool firstIteration = true;
    constexpr int blurPasses = 5;
    for (int i = 0; i < blurPasses; ++i) {
        const int index = horizontal ? 0 : 1;
        const int sourceIndex = horizontal ? 1 : 0;
        auto targetFbo = m_pingPongBuffers[index];
        auto sourceTexture = firstIteration
            ? m_brightFrameBuffer->GetColorTexture()
            : (m_pingPongBuffers[sourceIndex] ? m_pingPongBuffers[sourceIndex]->GetColorTexture() : nullptr);
        if (!targetFbo || !sourceTexture) {
            break;
        }
        targetFbo->Bind();
        glViewport(0, 0, m_width, m_height);
        m_blurShader->Bind();
        sourceTexture->Bind(0);
        m_blurShader->SetUniform("uImage", 0);
        m_blurShader->SetUniform("uHorizontal", horizontal ? 1 : 0);
        m_fullscreenQuad->Draw();
        m_blurShader->Unbind();
        targetFbo->Unbind();
        horizontal = !horizontal;
        if (firstIteration) {
            firstIteration = false;
        }
    }

    const int lastIndex = horizontal ? 1 : 0;
    if (m_pingPongBuffers[lastIndex]) {
        m_cachedBloomTexture = m_pingPongBuffers[lastIndex]->GetColorTexture();
    }
    if (!m_cachedBloomTexture && m_brightFrameBuffer) {
        m_cachedBloomTexture = m_brightFrameBuffer->GetColorTexture();
    }
    if (depthWasEnabled == GL_TRUE) {
        glEnable(GL_DEPTH_TEST);
    }
}
