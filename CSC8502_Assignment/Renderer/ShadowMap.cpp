/**
 * @file ShadowMap.cpp
 * @brief 实现用于生成方向光阴影贴图的 ShadowMap 封装类。
 */
#include "ShadowMap.h"

#include <glad/glad.h>

ShadowMap::ShadowMap(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
                     int width,
                     int height)
    : m_factory(factory)
    , m_fbo(nullptr)
    , m_depthTexture(nullptr)
    , m_lightViewProjection()
    , m_width(width)
    , m_height(height) {
    m_lightViewProjection.ToIdentity();
    RecreateResources(width, height);
}

void ShadowMap::Resize(int width, int height) {
    if (width == m_width && height == m_height) {
        return;
    }
    RecreateResources(width, height);
}

void ShadowMap::BeginCapture() {
    if (!m_fbo) {
        return;
    }
    m_fbo->Bind();
    glViewport(0, 0, m_width, m_height);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::EndCapture() {
    if (!m_fbo) {
        return;
    }
    m_fbo->Unbind();
}

std::shared_ptr<Engine::IAL::I_Texture> ShadowMap::GetDepthTexture() const {
    return m_depthTexture;
}

const Matrix4& ShadowMap::GetLightViewProjection() const {
    return m_lightViewProjection;
}

void ShadowMap::UpdateLight(const Vector3& lightPosition,
                            const Vector3& focusPoint,
                            float nearPlane,
                            float farPlane,
                            float orthoSize) {
    Matrix4 view = Matrix4::BuildViewMatrix(lightPosition, focusPoint, Vector3(0.0f, 1.0f, 0.0f));
    Matrix4 projection = Matrix4::Orthographic(nearPlane,
                                               farPlane,
                                               orthoSize,
                                               -orthoSize,
                                               orthoSize,
                                               -orthoSize);
    m_lightViewProjection = projection * view;
}

void ShadowMap::RecreateResources(int width, int height) {
    m_width = width;
    m_height = height;
    if (!m_factory) {
        m_fbo.reset();
        m_depthTexture.reset();
        return;
    }
    m_fbo = m_factory->CreateShadowFBO(width, height);
    m_depthTexture = m_fbo ? m_fbo->GetDepthTexture() : nullptr;
    ConfigureDepthTexture();
}

void ShadowMap::ConfigureDepthTexture() {
    if (!m_depthTexture) {
        return;
    }
    glBindTexture(GL_TEXTURE_2D, m_depthTexture->GetID());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}