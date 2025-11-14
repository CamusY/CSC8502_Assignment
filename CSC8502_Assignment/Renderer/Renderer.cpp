/**
* @file Renderer.cpp
 * @brief 实现负责遍历场景图并提交绘制命令的 Renderer 类。
 *
 * @details
 * 当前实现负责从场景图收集所有可渲染节点，并依次调用其网格接口执行 Draw，
 * 并结合 Camera::BuildViewMatrix 生成视图矩阵。Day11 阶段在此骨架上加入天空盒与
 * Blinn-Phong 光照，先渲染 cubemap 背景，再对地形节点注入光源与相机参数。
 */
#include "Renderer.h"

#include "PostProcessing.h"
#include "Water.h"
#include "../Core/Camera.h"
#include "../Engine/IAL/I_FrameBuffer.h"
#include "../Engine/IAL/I_Mesh.h"
#include "../Engine/IAL/I_Shader.h"
#include "../Engine/IAL/I_Texture.h"
#include "../Engine/IAL/I_AnimatedMesh.h"

#include <glad/glad.h>
#include <algorithm>
#include <iostream>
#include <vector>

namespace {
    constexpr int kMaxBones = 128;
}


Renderer::Renderer(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
                   const std::shared_ptr<SceneGraph>& sceneGraph,
                   const std::shared_ptr<Camera>& camera,
                   const std::shared_ptr<Engine::IAL::I_DebugUI>& debugUI,
                   int width,
                   int height) :
    m_factory(factory)
    , m_sceneGraph(sceneGraph)
    , m_camera(camera)
    , m_debugUI(debugUI)
    , m_postProcessing(nullptr)
    , m_sceneShader(nullptr)
    , m_terrainShader(nullptr)
    , m_skyboxShader(nullptr)
    , m_waterShader(nullptr)
    , m_shadowShader(nullptr)
    , m_skinnedShader(nullptr)
    , m_skyboxTexture(nullptr)
    , m_skyboxMesh(nullptr)
    , m_waterReflectionFBO(nullptr)
    , m_waterRefractionFBO(nullptr)
    , m_shadowMap(nullptr)
    , m_water(nullptr)
    , m_directionalLight{}
    , m_sceneColour(Vector3(0.0f, 0.0f, 0.0f))
    , m_specularPower(32.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(900.0f)
    , m_surfaceWidth(width)
    , m_surfaceHeight(height)
    , m_transitionEnabled(false)
    , m_transitionProgress(0.0f)
    , m_shadowMatrix()
    , m_reflectionViewProj()
    , m_shadowStrength(0.65f) {
    if (m_factory) {
        m_sceneShader = m_factory->CreateShader("Shared/basic.vert", "Shared/basic.frag");
        m_terrainShader = m_factory->CreateShader("Shared/terrain.vert", "Shared/terrain.frag");
        m_skyboxShader = m_factory->CreateShader("Shared/skybox.vert", "Shared/skybox.frag");
        m_waterShader = m_factory->CreateShader("Shared/water.vert", "Shared/water.frag");
        m_shadowShader = m_factory->CreateShader("Shared/shadow.vert", "Shared/shadow.frag");
        m_skinnedShader = m_factory->CreateShader("Shared/skinning.vert", "Shared/skinning.frag");
        m_skyboxMesh = m_factory->LoadMesh("../Meshes/cube.gltf");
        m_postProcessing = std::make_shared<PostProcessing>(m_factory, width, height);
        m_shadowMap = std::make_shared<ShadowMap>(m_factory, 2048, 2048);
    }

    m_shadowMatrix.ToIdentity();
    m_reflectionViewProj.ToIdentity();
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void Renderer::Render(float deltaTime) {
    if (!m_sceneGraph) {
        RenderDebugUI();
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CLIP_DISTANCE0);

    UpdateAnimatedMeshes(deltaTime);

    const Vector3 defaultCameraPos(0.0f, 0.0f, 10.5f);
    const Vector3 cameraPosition = m_camera ? m_camera->GetPosition() : defaultCameraPos;
    const float cameraYaw = m_camera ? m_camera->GetYaw() : 0.0f;
    const float cameraPitch = m_camera ? m_camera->GetPitch() : 0.0f;

    Matrix4 view = m_camera
        ? m_camera->BuildViewMatrix()
        : Matrix4::BuildViewMatrix(defaultCameraPos, Vector3(0.0f, 0.0f, 0.0f));
    const float aspect = m_surfaceHeight > 0
        ? static_cast<float>(m_surfaceWidth) / static_cast<float>(m_surfaceHeight)
        : 1.0f;
    Matrix4 projection = Matrix4::Perspective(m_nearPlane, m_farPlane, aspect, 35.0f);

    Matrix4 lightMatrix;
    lightMatrix.ToIdentity();
    if (m_shadowMap && m_shadowShader) {
        const Vector3 focusPoint(512.0f, 0.0f, 512.0f);
        const float shadowNear = 1.0f;
        const float shadowFar = 1000.0f;
        const float orthoSize = 512.0f;
        m_shadowMap->UpdateLight(m_directionalLight.position,
                                 focusPoint,
                                 shadowNear,
                                 shadowFar,
                                 orthoSize);
        m_shadowMap->BeginCapture();
        GLboolean cullEnabled = glIsEnabled(GL_CULL_FACE);
        GLint previousCull = GL_BACK;
        if (cullEnabled) {
            glGetIntegerv(GL_CULL_FACE_MODE, &previousCull);
        }
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        RenderSceneForShadowMap(m_shadowMap->GetLightViewProjection(), true);
        if (cullEnabled) {
            glCullFace(previousCull);
        }
        else {
            glDisable(GL_CULL_FACE);
        }
        m_shadowMap->EndCapture();
        glViewport(0, 0, m_surfaceWidth, m_surfaceHeight);
        lightMatrix = m_shadowMap->GetLightViewProjection();
    }
    m_shadowMatrix = lightMatrix;

    if (m_water && m_waterReflectionFBO && m_waterRefractionFBO) {
        RenderReflectionPass(projection, cameraPosition, cameraYaw, cameraPitch);
        RenderRefractionPass(view, projection, cameraPosition);
    }

    if (m_postProcessing) {
        m_postProcessing->BeginCapture();
    }

    RenderSkybox(view, projection);
    RenderScenePass(view, projection, cameraPosition, true);
    RenderWaterSurface(view, projection, cameraPosition);

    if (m_postProcessing) {
        m_postProcessing->EndCapture();
        m_postProcessing->Present(m_transitionEnabled, m_transitionProgress);
    }
    RenderDebugUI();
}

void Renderer::SetWater(const std::shared_ptr<Water>& water) {
    m_water = water;
    if (!m_factory || !m_water) {
        m_waterReflectionFBO.reset();
        m_waterRefractionFBO.reset();
        return;
    }
    m_waterReflectionFBO = m_factory->CreatePostProcessFBO(m_surfaceWidth, m_surfaceHeight);
    m_waterRefractionFBO = m_factory->CreatePostProcessFBO(m_surfaceWidth, m_surfaceHeight);
}

void Renderer::SetSkyboxTexture(const std::shared_ptr<Engine::IAL::I_Texture>& texture) {
    m_skyboxTexture = texture;
}

void Renderer::SetSceneColour(const Vector3& colour) {
    m_sceneColour = colour;
}

void Renderer::SetDirectionalLight(const Light& light) {
    m_directionalLight = light;
}

void Renderer::SetTransitionState(bool enabled, float progress) {
    m_transitionEnabled = enabled;
    m_transitionProgress = std::clamp(progress, 0.0f, 1.0f);
}

void Renderer::UpdateAnimatedMeshes(float deltaTime) {
    if (!m_sceneGraph) {
        return;
    }
    std::vector<std::shared_ptr<SceneNode>> nodes;
    m_sceneGraph->CollectRenderableNodes(nodes);
    for (const auto& node : nodes) {
        if (!node) {
            continue;
        }
        auto mesh = node->GetMesh();
        if (!mesh) {
            continue;
        }
        auto animated = std::dynamic_pointer_cast<Engine::IAL::I_AnimatedMesh>(mesh);
        if (animated) {
            animated->UpdateAnimation(deltaTime);
        }
    }
}

void Renderer::RenderSceneForShadowMap(const Matrix4& lightViewProjection,
                                       bool skipWaterNode) {
    if (!m_sceneGraph || !m_shadowShader) {
        return;
    }
    m_renderQueue.clear();
    m_sceneGraph->CollectRenderableNodes(m_renderQueue);
    m_shadowShader->Bind();
    m_shadowShader->SetUniform("uLightViewProj", lightViewProjection);
    for (const auto& node : m_renderQueue) {
        if (!node) {
            continue;
        }
        if (skipWaterNode && m_water && node == m_water->GetNode()) {
            continue;
        }
        auto mesh = node->GetMesh();
        if (!mesh) {
            continue;
        }
        auto animatedMesh = std::dynamic_pointer_cast<Engine::IAL::I_AnimatedMesh>(mesh);
        int boneCount = 0;
        if (animatedMesh) {
            const auto& bones = animatedMesh->GetBoneTransforms();
            boneCount = static_cast<int>(std::min<size_t>(bones.size(), static_cast<size_t>(kMaxBones)));
            if (boneCount > 0) {
                m_shadowShader->SetUniformMatrix4Array("uBoneMatrices", bones.data(), static_cast<size_t>(boneCount));
            }
        }
        m_shadowShader->SetUniform("uBoneCount", boneCount);
        Matrix4 modelMatrix = node->GetWorldTransform();
        if (animatedMesh) {
            modelMatrix = modelMatrix * animatedMesh->GetRootTransform();
        }
        m_shadowShader->SetUniform("uModel", modelMatrix);
        mesh->Draw();
    }
    m_shadowShader->Unbind();
}

void Renderer::RenderSkybox(const Matrix4& view, const Matrix4& projection) {
    if (!m_skyboxShader || !m_skyboxTexture || !m_skyboxMesh) {
        return;
    }
    Matrix4 viewNoTranslation = view;
    viewNoTranslation.values[12] = 0.0f;
    viewNoTranslation.values[13] = 0.0f;
    viewNoTranslation.values[14] = 0.0f;
    Matrix4 skyboxMatrix = projection * viewNoTranslation;

    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    m_skyboxShader->Bind();
    m_skyboxShader->SetUniform("uViewProj", skyboxMatrix);
    m_skyboxTexture->Bind(0);
    m_skyboxShader->SetUniform("uSkybox", 0);
    m_skyboxMesh->Draw();
    m_skyboxShader->Unbind();
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}

void Renderer::RenderScenePass(const Matrix4& view,
                               const Matrix4& projection,
                               const Vector3& cameraPosition,
                               bool skipWaterNode,
                               const Vector4* clipPlane) {
    if (!m_sceneGraph) {
        return;
    }
    m_renderQueue.clear();
    m_sceneGraph->CollectRenderableNodes(m_renderQueue);
    
    const float fogDensity = 0.0018f;
    Vector3 fogColor = m_directionalLight.ambient * 0.4f + Vector3(0.25f, 0.30f, 0.40f) * 0.6f;
    
    Matrix4 viewProj = projection * view;
    Vector4 clip(0.0f, 0.0f, 0.0f, 0.0f);
    if (clipPlane) {
        clip = *clipPlane;
        glEnable(GL_CLIP_DISTANCE0);
    }
    else {
        glDisable(GL_CLIP_DISTANCE0);
    }

    for (const auto& node : m_renderQueue) {
        if (!node) {
            continue;
        }
        if (skipWaterNode && m_water && node == m_water->GetNode()) {
            continue;
        }
        auto mesh = node->GetMesh();
        if (!mesh) {
            continue;
        }
        auto animatedMesh = std::dynamic_pointer_cast<Engine::IAL::I_AnimatedMesh>(mesh);
        Matrix4 modelMatrix = node->GetWorldTransform();
        if (animatedMesh) {
            modelMatrix = modelMatrix * animatedMesh->GetRootTransform();
        }
        auto shadowTexture = m_shadowMap ? m_shadowMap->GetDepthTexture() : nullptr;
        const bool hasShadow = static_cast<bool>(shadowTexture);
        if (animatedMesh && m_skinnedShader) {
            const auto& bones = animatedMesh->GetBoneTransforms();
            const int boneCount = static_cast<int>(std::min<size_t>(bones.size(), static_cast<size_t>(kMaxBones)));
            m_skinnedShader->Bind();
            m_skinnedShader->SetUniform("uViewProj", viewProj);
            m_skinnedShader->SetUniform("uModel", node->GetWorldTransform());
            m_skinnedShader->SetUniform("uClipPlane", clip);
            m_skinnedShader->SetUniform("uLightPosition", m_directionalLight.position);
            m_skinnedShader->SetUniform("uLightColor", m_directionalLight.color);
            m_skinnedShader->SetUniform("uAmbientColor", m_directionalLight.ambient);
            m_skinnedShader->SetUniform("uCameraPos", cameraPosition);
            m_skinnedShader->SetUniform("uSpecularPower", m_specularPower);
            m_skinnedShader->SetUniform("uShadowMatrix", m_shadowMatrix);
            m_skinnedShader->SetUniform("uShadowStrength", hasShadow ? m_shadowStrength : 0.0f);
            m_skinnedShader->SetUniform("uBoneCount", boneCount);
            if (boneCount > 0) {
                m_skinnedShader->SetUniformMatrix4Array("uBoneMatrices", bones.data(), static_cast<size_t>(boneCount));
            }
            if (hasShadow) {
                shadowTexture->Bind(2);
                m_skinnedShader->SetUniform("uShadowMap", 2);
            }
            auto texture = node->GetTexture();
            if (!texture) {
                texture = mesh->GetDefaultTexture();
            }
            if (texture) {
                texture->Bind(0);
                m_skinnedShader->SetUniform("uDiffuse", 0);
                m_skinnedShader->SetUniform("uHasTexture", 1);
            }
            else {
                m_skinnedShader->SetUniform("uHasTexture", 0);
                m_skinnedShader->SetUniform("uBaseColor", m_sceneColour);
            }
            mesh->Draw();
            m_skinnedShader->Unbind();
            continue;
        }
        auto texture = node->GetTexture();
        if (!texture) {
            texture = mesh->GetDefaultTexture();
        }
        std::shared_ptr<Engine::IAL::I_Shader> shader = texture ? m_terrainShader : m_sceneShader;
        if (!shader) {
            continue;
        }
        shader->Bind();
        shader->SetUniform("uViewProj", viewProj);
        shader->SetUniform("uModel", modelMatrix);
        shader->SetUniform("uModel", node->GetWorldTransform());
        shader->SetUniform("uClipPlane", clip);
        shader->SetUniform("uLightPosition", m_directionalLight.position);
        shader->SetUniform("uLightColor", m_directionalLight.color);
        
        shader->SetUniform("uFogColor", fogColor);
        shader->SetUniform("uFogDensity", fogDensity);
        
        shader->SetUniform("uAmbientColor", m_directionalLight.ambient);
        shader->SetUniform("uShadowMatrix", m_shadowMatrix);
        shader->SetUniform("uShadowStrength", hasShadow ? m_shadowStrength : 0.0f);
        if (hasShadow) {
            shadowTexture->Bind(2);
            shader->SetUniform("uShadowMap", 2);
        }
        if (texture) {
            texture->Bind(0);
            shader->SetUniform("uDiffuse", 0);
            shader->SetUniform("uSpecularPower", m_specularPower);
            shader->SetUniform("uCameraPos", cameraPosition);
        }
        else {
            shader->SetUniform("uColor", m_sceneColour);
            shader->SetUniform("uSpecularPower", m_specularPower);
            shader->SetUniform("uCameraPos", cameraPosition);
        }
        mesh->Draw();
        shader->Unbind();
    }
    glDisable(GL_CLIP_DISTANCE0);
}

void Renderer::RenderWaterSurface(const Matrix4& view,
                                  const Matrix4& projection,
                                  const Vector3& cameraPosition) {
    if (!m_water || !m_waterShader) {
        return;
    }
    auto waterNode = m_water->GetNode();
    if (!waterNode) {
        return;
    }
    auto reflectionTexture = m_waterReflectionFBO ? m_waterReflectionFBO->GetColorTexture() : nullptr;
    auto refractionTexture = m_waterRefractionFBO ? m_waterRefractionFBO->GetColorTexture() : nullptr;
    if (!reflectionTexture || !refractionTexture) {
        return;
    }
    auto mesh = waterNode->GetMesh();
    if (!mesh) {
        return;
    }

    Matrix4 viewProj = projection * view;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    m_waterShader->Bind();
    m_waterShader->SetUniform("uReflectionViewProj", m_reflectionViewProj);
    m_waterShader->SetUniform("uViewProj", viewProj);
    m_waterShader->SetUniform("uModel", waterNode->GetWorldTransform());
    m_waterShader->SetUniform("uCameraPos", cameraPosition);
    m_waterShader->SetUniform("uLightColor", m_directionalLight.color);
    m_waterShader->SetUniform("uAmbientColor", m_directionalLight.ambient);
    auto shadowTexture = m_shadowMap ? m_shadowMap->GetDepthTexture() : nullptr;
    const bool hasShadow = static_cast<bool>(shadowTexture);
    m_waterShader->SetUniform("uShadowMatrix", m_shadowMatrix);
    m_waterShader->SetUniform("uShadowStrength", hasShadow ? m_shadowStrength : 0.0f);
    if (hasShadow) {
        shadowTexture->Bind(2);
        m_waterShader->SetUniform("uShadowMap", 2);
    }
    reflectionTexture->Bind(0);
    m_waterShader->SetUniform("uReflectionTex", 0);
    refractionTexture->Bind(1);
    m_waterShader->SetUniform("uRefractionTex", 1);
    mesh->Draw();
    m_waterShader->Unbind();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

}

void Renderer::RenderReflectionPass(const Matrix4& projection,
                                    const Vector3& cameraPosition,
                                    float cameraYaw,
                                    float cameraPitch) {
    if (!m_water || !m_waterReflectionFBO) {
        return;
    }
    float distance = cameraPosition.y - m_water->GetHeight();
    Vector3 reflectedPosition = cameraPosition;
    reflectedPosition.y -= distance * 2.0f;

    Camera reflectionCamera;
    reflectionCamera.SetPosition(reflectedPosition);
    reflectionCamera.SetYaw(cameraYaw);
    reflectionCamera.SetPitch(-cameraPitch);

    m_waterReflectionFBO->Bind();
    glViewport(0, 0, m_surfaceWidth, m_surfaceHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Matrix4 reflectionView = reflectionCamera.BuildViewMatrix();
    m_reflectionViewProj = projection * reflectionView;
    RenderSkybox(reflectionView, projection);
    const float waterHeight = m_water->GetHeight();
    constexpr float clipBias = 0.5f;
    Vector4 reflectionClip(0.0f, 1.0f, 0.0f, -(waterHeight - clipBias));
    RenderScenePass(reflectionView, projection, reflectionCamera.GetPosition(), true, &reflectionClip);

    m_waterReflectionFBO->Unbind();
}

void Renderer::RenderRefractionPass(const Matrix4& view,
                                    const Matrix4& projection,
                                    const Vector3& cameraPosition) {
    if (!m_water || !m_waterRefractionFBO) {
        return;
    }
    m_waterRefractionFBO->Bind();
    glViewport(0, 0, m_surfaceWidth, m_surfaceHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderSkybox(view, projection);
    const float waterHeight = m_water->GetHeight();
    constexpr float clipBias = 0.5f;
    Vector4 refractionClip(0.0f, -1.0f, 0.0f, waterHeight + clipBias);
    RenderScenePass(view, projection, cameraPosition, true, &refractionClip);

    m_waterRefractionFBO->Unbind();
}

void Renderer::RenderDebugUI() {
    if (!m_debugUI) {
        return;
    }
    if (m_debugUI->BeginWindow("Lighting Controls")) {
        Vector3 lightPosition = m_directionalLight.position;
        if (m_debugUI->SliderFloat3("Directional Position", &lightPosition, -5000.0f, 5000.0f)) {
            m_directionalLight.position = lightPosition;
        }

        Vector3 lightColour = m_directionalLight.color;
        if (m_debugUI->ColorEdit3("Directional Colour", &lightColour)) {
            m_directionalLight.color = lightColour;
        }

        Vector3 ambientColour = m_directionalLight.ambient;
        if (m_debugUI->ColorEdit3("Ambient Colour", &ambientColour)) {
            m_directionalLight.ambient = ambientColour;
        }

        float specular = m_specularPower;
        if (m_debugUI->SliderFloat("Specular Power", &specular, 1.0f, 256.0f)) {
            m_specularPower = specular;
        }
    }
    m_debugUI->EndWindow();
}
