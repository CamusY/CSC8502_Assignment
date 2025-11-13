
/**
 * @file ShadowMap.h
 * @brief 声明用于生成方向光阴影贴图的 ShadowMap 封装类。
 * @details
 * ShadowMap 负责通过资源工厂创建阴影专用的帧缓冲对象，
 * 维护光源视图投影矩阵，并提供捕获深度图的生命周期函数。
 * 渲染器在 Day16 阶段使用该类完成阴影预渲染，并在主场景着色
 * 时复用深度纹理与光源矩阵以计算阴影遮蔽。
 */
#pragma once

#include <memory>

#include "../Engine/IAL/I_FrameBuffer.h"
#include "../Engine/IAL/I_ResourceFactory.h"

#include "nclgl/Matrix4.h"
#include "nclgl/Vector3.h"

class ShadowMap {
public:
    ShadowMap(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
              int width,
              int height);

    void Resize(int width, int height);

    void BeginCapture();
    void EndCapture();

    std::shared_ptr<Engine::IAL::I_Texture> GetDepthTexture() const;
    const Matrix4& GetLightViewProjection() const;

    void UpdateLight(const Vector3& lightPosition,
                     const Vector3& focusPoint,
                     float nearPlane,
                     float farPlane,
                     float orthoSize);

private:
    void RecreateResources(int width, int height);
    void ConfigureDepthTexture();

    std::shared_ptr<Engine::IAL::I_ResourceFactory> m_factory;
    std::shared_ptr<Engine::IAL::I_FrameBuffer> m_fbo;
    std::shared_ptr<Engine::IAL::I_Texture> m_depthTexture;
    Matrix4 m_lightViewProjection;
    int m_width;
    int m_height;
};