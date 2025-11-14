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
#include "GrassField.h"
#include "RainSystem.h"
#include "../Core/Camera.h"
#include "../Core/TerrainConfig.h"
#include "../Engine/IAL/I_FrameBuffer.h"
#include "../Engine/IAL/I_Mesh.h"
#include "../Engine/IAL/I_Shader.h"
#include "../Engine/IAL/I_Texture.h"
#include "../Engine/IAL/I_AnimatedMesh.h"
#include "../Engine/IAL/I_Heightmap.h"

#include <glad/glad.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include "nclgl/Vector2.h"

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
    , m_nearPlane(0.3f)
    , m_farPlane(std::max(1500.0f, kTerrainExtent * 1.1f))
    , m_surfaceWidth(width)
    , m_surfaceHeight(height)
    , m_transitionEnabled(false)
    , m_transitionProgress(0.0f)
    , m_shadowMatrix()
    , m_reflectionViewProj()
    , m_shadowStrength(0.65f)
    , m_bonePaletteBuffer(0)
    , m_boneCapacity(0)
    , m_environmentIntensity(1.0f)
    , m_environmentMaxLod(5.0f)
    , m_activeHeightmap(nullptr)
    , m_grassField(nullptr)
    , m_rainSystem(nullptr)
    , m_grassBaseTextureOverride(nullptr)
    , m_timeAccumulator(0.0f)
    , m_grassEnabled(true)
    , m_rainEnabled(false)
    , m_viewLayout(ViewLayoutMode::Single)
    , m_defaultViewMode(RenderDebugMode::Standard)
    , m_splitCameras() {
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
    if (m_factory) {
        m_rainSystem = std::make_unique<RainSystem>(m_factory, 2000, 240.0f, 160.0f);
    }
    for (auto& cam : m_splitCameras) {
        cam = std::make_shared<Camera>();
        if (cam) {
            cam->SetMode(Camera::Mode::Free);
        }
    }

    UpdateSplitViewCameras();

    m_shadowMatrix.ToIdentity();
    m_reflectionViewProj.ToIdentity();
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

Renderer::~Renderer() {
    if (m_bonePaletteBuffer != 0) {
        glDeleteBuffers(1, &m_bonePaletteBuffer);
        m_bonePaletteBuffer = 0;
        m_boneCapacity = 0;
    }
}

void Renderer::Render(float deltaTime) {
    if (!m_sceneGraph) {
        RenderDebugUI();
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CLIP_DISTANCE0);


    const Vector3 defaultCameraPos(0.0f, 0.0f, 10.5f);
    const Vector3 cameraPosition = m_camera ? m_camera->GetPosition() : defaultCameraPos;
    const float cameraYaw = m_camera ? m_camera->GetYaw() : 0.0f;
    const float cameraPitch = m_camera ? m_camera->GetPitch() : 0.0f;

    UpdateAnimatedMeshes(deltaTime);
    m_timeAccumulator += deltaTime;

    if (m_rainSystem && m_rainEnabled) {
        float waterLevel = m_water ? m_water->GetHeight() : 0.0f;
        m_rainSystem->Update(deltaTime,
                             cameraPosition,
                             cameraYaw,
                             cameraPitch,
                             m_activeHeightmap,
                             waterLevel);
    }


    Matrix4 lightMatrix;
    lightMatrix.ToIdentity();
    if (m_shadowMap && m_shadowShader) {
        const Vector3 focusPoint = GetSceneFocusPoint();
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


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_surfaceWidth, m_surfaceHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_viewLayout == ViewLayoutMode::Single) {
        RenderSingleView(deltaTime);
    }
    else {
        RenderQuadView(deltaTime);
    }
    RenderDebugUI();
}

void Renderer::SetWater(const std::shared_ptr<Water>& water) {
    m_water = water;
    if (!m_factory) {
        m_waterReflectionFBO.reset();
        m_waterRefractionFBO.reset();
        m_grassField.reset();
        return;
    }
    if (!m_water) {
        m_waterReflectionFBO.reset();
        m_waterRefractionFBO.reset();
        SetTerrainHeightmap(m_activeHeightmap);
        return;
    }
    m_waterReflectionFBO = m_factory->CreatePostProcessFBO(m_surfaceWidth, m_surfaceHeight);
    m_waterRefractionFBO = m_factory->CreatePostProcessFBO(m_surfaceWidth, m_surfaceHeight);
    SetTerrainHeightmap(m_activeHeightmap);
    UpdateSplitViewCameras();
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

void Renderer::SetTerrainHeightmap(const std::shared_ptr<Engine::IAL::I_Heightmap>& heightmap) {
    m_activeHeightmap = heightmap;
    UpdateViewRangeFromTerrain();
    if (!m_factory) {
        m_grassField.reset();
        return;
    }
    if (!m_activeHeightmap || !m_grassEnabled) {
        m_grassField.reset();
        return;
    }
    const float waterHeight = m_water ? m_water->GetHeight() : 0.0f;
    m_grassField = std::make_unique<GrassField>(m_factory, m_activeHeightmap, waterHeight);
    if (m_grassField) {
        m_grassField->SetBaseColorTexture(m_grassBaseTextureOverride);
    }
    UpdateSplitViewCameras();
}

void Renderer::SetGrassEnabled(bool enabled) {
    if (m_grassEnabled == enabled) {
        return;
    }
    m_grassEnabled = enabled;
    if (!m_grassEnabled) {
        m_grassField.reset();
        return;
    }
    if (m_factory && m_activeHeightmap) {
        const float waterHeight = m_water ? m_water->GetHeight() : 0.0f;
        m_grassField = std::make_unique<GrassField>(m_factory, m_activeHeightmap, waterHeight);
        if (m_grassField) {
            m_grassField->SetBaseColorTexture(m_grassBaseTextureOverride);
        }
    }
}

void Renderer::SetGrassBaseTexture(const std::shared_ptr<Engine::IAL::I_Texture>& texture) {
    m_grassBaseTextureOverride = texture;
    if (m_grassField) {
        m_grassField->SetBaseColorTexture(m_grassBaseTextureOverride);
    }
}

void Renderer::SetRainEnabled(bool enabled) {
    m_rainEnabled = enabled;
}

void Renderer::SetDefaultViewMode(RenderDebugMode mode) {
    m_defaultViewMode = mode;
}

void Renderer::ToggleMultiViewLayout() {
    m_viewLayout = (m_viewLayout == ViewLayoutMode::Single) ? ViewLayoutMode::Quad : ViewLayoutMode::Single;
    UpdateSplitViewCameras();
}

void Renderer::OnSurfaceResized(int width, int height) {
    if (width <= 0 || height <= 0) {
        return;
    }
    m_surfaceWidth = width;
    m_surfaceHeight = height;
    if (m_postProcessing) {
        m_postProcessing->Resize(width, height);
    }
    if (m_shadowMap) {
        glViewport(0, 0, m_surfaceWidth, m_surfaceHeight);
    }
    if (m_water) {
        SetWater(m_water);
    }
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
            boneCount = static_cast<int>(bones.size());
            BindBonePalette(bones, boneCount);
        }
        else {
            UnbindBonePalette();
        }
        m_shadowShader->SetUniform("uBoneCount", boneCount);
        Matrix4 modelMatrix = node->GetWorldTransform();
        if (animatedMesh) {
            modelMatrix = modelMatrix * animatedMesh->GetRootTransform();
        }
        m_shadowShader->SetUniform("uModel", modelMatrix);
        mesh->Draw();
    }
    UnbindBonePalette();
    m_shadowShader->Unbind();
}

void Renderer::RenderSingleView(float /*deltaTime*/) {
    std::shared_ptr<Camera> cameraToUse = m_camera;
    if (!cameraToUse) {
        cameraToUse = std::make_shared<Camera>();
        cameraToUse->SetPosition(Vector3(0.0f, 0.0f, 10.5f));
        cameraToUse->SetYaw(0.0f);
        cameraToUse->SetPitch(0.0f);
    }
    RenderViewInternal(cameraToUse, m_defaultViewMode, 0, 0, m_surfaceWidth, m_surfaceHeight, true);
}

void Renderer::RenderQuadView(float /*deltaTime*/) {
    UpdateSplitViewCameras();
    const int halfWidth = std::max(1, m_surfaceWidth / 2);
    const int halfHeight = std::max(1, m_surfaceHeight / 2);
    const int topHeight = m_surfaceHeight - halfHeight;
    const int rightWidth = m_surfaceWidth - halfWidth;

    std::shared_ptr<Camera> primary = m_camera;
    if (!primary) {
        primary = std::make_shared<Camera>();
        primary->SetPosition(Vector3(0.0f, 0.0f, 10.5f));
        primary->SetYaw(0.0f);
        primary->SetPitch(0.0f);
    }

    RenderViewInternal(primary, m_defaultViewMode, 0, halfHeight, halfWidth, topHeight, true);
    RenderViewInternal(m_splitCameras[0], RenderDebugMode::Wireframe, halfWidth, halfHeight, std::max(1, rightWidth),
                       topHeight, false);
    RenderViewInternal(m_splitCameras[1], RenderDebugMode::Normal, 0, 0, halfWidth, halfHeight, false);
    RenderViewInternal(m_splitCameras[2], RenderDebugMode::Bloom, halfWidth, 0, std::max(1, rightWidth), halfHeight,
                       false);
}

void Renderer::RenderViewInternal(const std::shared_ptr<Camera>& camera,
                                  RenderDebugMode mode,
                                  int viewportX,
                                  int viewportY,
                                  int viewportWidth,
                                  int viewportHeight,
                                  bool allowTransition) {
    if (!camera || !m_postProcessing) {
        return;
    }
    const int width = std::max(1, viewportWidth);
    const int height = std::max(1, viewportHeight);
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    Matrix4 view = camera->BuildViewMatrix();
    Matrix4 projection = Matrix4::Perspective(m_nearPlane, m_farPlane, aspect, 35.0f);
    const Vector3 cameraPosition = camera->GetPosition();
    const float cameraYaw = camera->GetYaw();
    const float cameraPitch = camera->GetPitch();

    if (m_water && m_waterReflectionFBO && m_waterRefractionFBO) {
        RenderReflectionPass(projection, cameraPosition, cameraYaw, cameraPitch);
        RenderRefractionPass(view, projection, cameraPosition);
    }

    if (m_postProcessing) {
        m_postProcessing->BeginCapture();
    }

    int prevFront = GL_FILL;
    int prevBack = GL_FILL;
    ApplyPolygonMode(mode, prevFront, prevBack);

    const bool skipSkybox = (mode == RenderDebugMode::Normal || mode == RenderDebugMode::Depth);
    if (!skipSkybox) {
        RenderSkybox(view, projection);
    }

    RenderScenePass(view, projection, cameraPosition, true, mode);
    RenderGrass(view, projection, cameraPosition, mode);
    RenderWaterSurface(view, projection, cameraPosition, mode);
    RenderRain(view, projection, cameraPosition, cameraYaw, cameraPitch, mode);

    RestorePolygonMode(mode, prevFront, prevBack);

    if (m_postProcessing) {
        m_postProcessing->EndCapture();
        PostProcessing::OutputMode outputMode = PostProcessing::OutputMode::ToneMapped;
        if (mode == RenderDebugMode::Bloom) {
            outputMode = PostProcessing::OutputMode::BloomOnly;
        }
        else if (mode != RenderDebugMode::Standard) {
            outputMode = PostProcessing::OutputMode::RawScene;
        }
        const bool transition = allowTransition && m_transitionEnabled && mode == RenderDebugMode::Standard;
        m_postProcessing->PresentToViewport(outputMode,
                                            transition,
                                            m_transitionProgress,
                                            viewportX,
                                            viewportY,
                                            width,
                                            height);
    }
}

int Renderer::ToShaderDebugMode(RenderDebugMode mode) const {
    switch (mode) {
    case RenderDebugMode::Normal:
        return 1;
    case RenderDebugMode::Depth:
        return 2;
    default:
        return 0;
    }
}

void Renderer::ApplyPolygonMode(RenderDebugMode mode, int& previousFront, int& previousBack) const {
    GLint polygonMode = GL_FILL;
    glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
    previousFront = polygonMode;
    previousBack = polygonMode;
    if (mode == RenderDebugMode::Wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Renderer::RestorePolygonMode(RenderDebugMode /*mode*/, int previousFront, int /*previousBack*/) const {
    glPolygonMode(GL_FRONT_AND_BACK, previousFront);
}

void Renderer::UpdateSplitViewCameras() {
    const Vector3 focus = GetSceneFocusPoint();
    const float waterLevel = m_water ? m_water->GetHeight() : focus.y;
    if (m_splitCameras[0]) {
        m_splitCameras[0]->SetPosition(focus + Vector3(-320.0f, 180.0f, -260.0f));
        m_splitCameras[0]->SetYaw(45.0f);
        m_splitCameras[0]->SetPitch(-20.0f);
    }
    if (m_splitCameras[1]) {
        m_splitCameras[1]->SetPosition(Vector3(focus.x, waterLevel + 420.0f, focus.z));
        m_splitCameras[1]->SetYaw(0.0f);
        m_splitCameras[1]->SetPitch(-89.0f);
    }
    if (m_splitCameras[2]) {
        m_splitCameras[2]->SetPosition(focus + Vector3(320.0f, 150.0f, 240.0f));
        m_splitCameras[2]->SetYaw(-135.0f);
        m_splitCameras[2]->SetPitch(-18.0f);
    }
}

Vector3 Renderer::GetSceneFocusPoint() const {
    Vector3 focus(512.0f, 0.0f, 512.0f);
    if (m_activeHeightmap) {
        const Vector2 resolution = m_activeHeightmap->GetResolution();
        const Vector3 scale = m_activeHeightmap->GetWorldScale();
        focus.x = (resolution.x > 1.0f) ? (resolution.x - 1.0f) * scale.x * 0.5f : 0.0f;
        focus.z = (resolution.y > 1.0f) ? (resolution.y - 1.0f) * scale.z * 0.5f : 0.0f;
        focus.y = m_activeHeightmap->SampleHeight(focus.x, focus.z);
    }
    if (m_water) {
        focus.y = m_water->GetHeight();
    }
    return focus;
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
                               RenderDebugMode mode,
                               const Vector4* clipPlane) {
    if (!m_sceneGraph) {
        return;
    }
    m_renderQueue.clear();
    m_sceneGraph->CollectRenderableNodes(m_renderQueue);

    const float fogDensity = GetFogDensity();
    Vector3 fogColor = GetFogColor();

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

        std::shared_ptr<Engine::IAL::I_Shader> shader;
        if (animatedMesh) {
            shader = m_skinnedShader;
        }
        else {
            shader = node->GetTexture() ? m_terrainShader : m_sceneShader;
        }
        if (!shader) {
            continue;
        }

        shader->Bind();
        shader->SetUniform("uViewProj", viewProj);
        shader->SetUniform("uView", view);
        shader->SetUniform("uModel", modelMatrix);
        shader->SetUniform("uClipPlane", clip);
        shader->SetUniform("uLightPosition", m_directionalLight.position);
        shader->SetUniform("uLightColor", m_directionalLight.color);
        shader->SetUniform("uAmbientColor", m_directionalLight.ambient);
        shader->SetUniform("uCameraPos", cameraPosition);
        shader->SetUniform("uFogColor", fogColor);
        shader->SetUniform("uFogDensity", fogDensity);
        shader->SetUniform("uShadowMatrix", m_shadowMatrix);
        shader->SetUniform("uShadowStrength", hasShadow ? m_shadowStrength : 0.0f);
        shader->SetUniform("uEnvironmentIntensity", m_environmentIntensity);
        shader->SetUniform("uEnvironmentMaxLod", m_environmentMaxLod);
        shader->SetUniform("uUseEnvironment", m_skyboxTexture ? 1 : 0);
        shader->SetUniform("uDebugMode", ToShaderDebugMode(mode));
        shader->SetUniform("uNearPlane", m_nearPlane);
        shader->SetUniform("uFarPlane", m_farPlane);

        Engine::IAL::PBRMaterial material = ResolveMaterial(node, mesh);
        shader->SetUniform("uBaseColorFactor", material.baseColorFactor);
        shader->SetUniform("uMetallicFactor", material.metallicFactor);
        shader->SetUniform("uRoughnessFactor", material.roughnessFactor);
        shader->SetUniform("uEmissiveFactor", material.emissiveFactor);
        shader->SetUniform("uAlphaCutoff", material.alphaCutoff);
        shader->SetUniform("uAlphaMode", ToAlphaModeValue(material.alphaMode));
        shader->SetUniform("uDoubleSided", material.doubleSided ? 1 : 0);

        int hasBase = material.baseColor ? 1 : 0;
        int hasNormal = material.normal ? 1 : 0;
        int hasMetallic = material.metallicRoughness ? 1 : 0;
        int hasAO = material.ambientOcclusion ? 1 : 0;
        int hasEmissive = material.emissive ? 1 : 0;
        shader->SetUniform("uHasBaseColorMap", hasBase);
        shader->SetUniform("uHasNormalMap", hasNormal);
        shader->SetUniform("uHasMetallicRoughnessMap", hasMetallic);
        shader->SetUniform("uHasAOMap", hasAO);
        shader->SetUniform("uHasEmissiveMap", hasEmissive);

        if (hasBase) {
            material.baseColor->Bind(0);
            shader->SetUniform("uBaseColorMap", 0);
        }
        if (hasNormal) {
            material.normal->Bind(1);
            shader->SetUniform("uNormalMap", 1);
        }
        if (hasMetallic) {
            material.metallicRoughness->Bind(2);
            shader->SetUniform("uMetallicRoughnessMap", 2);
        }
        if (hasAO) {
            material.ambientOcclusion->Bind(3);
            shader->SetUniform("uAOMap", 3);
        }
        if (hasEmissive) {
            material.emissive->Bind(4);
            shader->SetUniform("uEmissiveMap", 4);
        }
        if (m_skyboxTexture) {
            m_skyboxTexture->Bind(5);
            shader->SetUniform("uEnvironmentMap", 5);
        }
        if (hasShadow) {
            shadowTexture->Bind(6);
            shader->SetUniform("uShadowMap", 6);
        }

        GLboolean prevCull = glIsEnabled(GL_CULL_FACE);
        if (material.doubleSided) {
            glDisable(GL_CULL_FACE);
        }
        else if (!prevCull) {
            glEnable(GL_CULL_FACE);
        }

        GLboolean prevBlend = glIsEnabled(GL_BLEND);
        GLboolean prevDepthMask = GL_TRUE;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &prevDepthMask);
        const int alphaMode = ToAlphaModeValue(material.alphaMode);
        if (alphaMode == 2) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);
        }
        else {
            if (!prevBlend) {
                glDisable(GL_BLEND);
            }
            glDepthMask(GL_TRUE);
        }

        if (animatedMesh) {
            const auto& bones = animatedMesh->GetBoneTransforms();
            const int boneCount = static_cast<int>(bones.size());
            shader->SetUniform("uBoneCount", boneCount);
            BindBonePalette(bones, boneCount);
        }
        else {
            UnbindBonePalette();
        }

        mesh->Draw();

        if (material.doubleSided && prevCull) {
            glEnable(GL_CULL_FACE);
        }
        else if (!prevCull) {
            glDisable(GL_CULL_FACE);
        }

        if (alphaMode == 2) {
            if (!prevBlend) {
                glDisable(GL_BLEND);
            }
            glDepthMask(prevDepthMask);
        }

        shader->Unbind();
    }
    glDisable(GL_CLIP_DISTANCE0);
    UnbindBonePalette();
}

void Renderer::RenderGrass(const Matrix4& view,
                           const Matrix4& projection,
                           const Vector3& cameraPosition,
                           RenderDebugMode mode) {
    if (!m_grassEnabled || !m_grassField) {
        return;
    }
    const int debugMode = ToShaderDebugMode(mode);
    m_grassField->Render(view, projection, cameraPosition, m_timeAccumulator, debugMode, m_nearPlane, m_farPlane);
}

void Renderer::RenderWaterSurface(const Matrix4& view,
                                  const Matrix4& projection,
                                  const Vector3& cameraPosition,
                                  RenderDebugMode mode) {
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
    Matrix4 modelMatrix = waterNode->GetWorldTransform();
    const float fogDensity = GetFogDensity();
    Vector3 fogColor = GetFogColor();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    m_waterShader->Bind();
    m_waterShader->SetUniform("uModel", modelMatrix);
    m_waterShader->SetUniform("uReflectionViewProj", m_reflectionViewProj);
    m_waterShader->SetUniform("uViewProj", viewProj);
    m_waterShader->SetUniform("uCameraPos", cameraPosition);
    m_waterShader->SetUniform("uLightColor", m_directionalLight.color);
    m_waterShader->SetUniform("uAmbientColor", m_directionalLight.ambient);
    m_waterShader->SetUniform("uFogColor", fogColor);
    m_waterShader->SetUniform("uFogDensity", fogDensity);
    m_waterShader->SetUniform("uDebugMode", ToShaderDebugMode(mode));
    m_waterShader->SetUniform("uNearPlane", m_nearPlane);
    m_waterShader->SetUniform("uFarPlane", m_farPlane);

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
    RenderScenePass(reflectionView, projection, reflectionCamera.GetPosition(), true, RenderDebugMode::Standard,
                    &reflectionClip);

    m_waterReflectionFBO->Unbind();
}

void Renderer::BindBonePalette(const std::vector<Matrix4>& bones, int boneCount) {
    if (boneCount <= 0 || bones.empty()) {
        UnbindBonePalette();
        return;
    }

    EnsureBoneBufferCapacity(static_cast<std::size_t>(boneCount));

    if (m_bonePaletteBuffer == 0) {
        return;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_bonePaletteBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    0,
                    static_cast<GLsizeiptr>(boneCount * sizeof(Matrix4)),
                    bones.data());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_bonePaletteBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::UnbindBonePalette() {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
}

void Renderer::EnsureBoneBufferCapacity(std::size_t requiredCount) {
    if (requiredCount == 0) {
        return;
    }

    if (m_bonePaletteBuffer == 0) {
        glGenBuffers(1, &m_bonePaletteBuffer);
    }

    if (requiredCount <= m_boneCapacity) {
        return;
    }

    const std::size_t newCapacity = std::max<std::size_t>(requiredCount, std::max<std::size_t>(m_boneCapacity * 2, 64));
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_bonePaletteBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 static_cast<GLsizeiptr>(newCapacity * sizeof(Matrix4)),
                 nullptr,
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    m_boneCapacity = newCapacity;
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
    RenderScenePass(view, projection, cameraPosition, true, RenderDebugMode::Standard, &refractionClip);

    m_waterRefractionFBO->Unbind();
}

void Renderer::RenderRain(const Matrix4& view,
                          const Matrix4& projection,
                          const Vector3& cameraPosition,
                          float cameraYaw,
                          float cameraPitch,
                          RenderDebugMode mode) {
    if (!m_rainSystem || !m_rainEnabled || mode != RenderDebugMode::Standard) {
        return;
    }
    const float fogDensity = GetFogDensity();
    Vector3 fogColor = GetFogColor();
    m_rainSystem->Render(view, projection, cameraPosition, cameraYaw, cameraPitch, fogColor, fogDensity);
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

Engine::IAL::PBRMaterial Renderer::ResolveMaterial(const std::shared_ptr<SceneNode>& node,
                                                   const std::shared_ptr<Engine::IAL::I_Mesh>& mesh) const {
    Engine::IAL::PBRMaterial material;
    if (mesh) {
        if (const auto* existing = mesh->GetPBRMaterial()) {
            material = *existing;
        }
    }
    if (!material.baseColorFactor.x && !material.baseColorFactor.y && !material.baseColorFactor.z) {
        material.baseColorFactor = Vector4(m_sceneColour.x, m_sceneColour.y, m_sceneColour.z, 1.0f);
    }
    material.metallicFactor = std::max(material.metallicFactor, 0.0f);
    if (material.roughnessFactor <= 0.0f) {
        material.roughnessFactor = 1.0f;
    }
    if (node) {
        if (auto texture = node->GetTexture()) {
            material.baseColor = texture;
        }
    }
    return material;
}

int Renderer::ToAlphaModeValue(Engine::IAL::AlphaMode mode) {
    switch (mode) {
    case Engine::IAL::AlphaMode::Mask:
        return 1;
    case Engine::IAL::AlphaMode::Blend:
        return 2;
    default:
        return 0;
    }
}

float Renderer::GetTerrainExtent() const {
    if (!m_activeHeightmap) {
        return 0.0f;
    }
    const Vector2 resolution = m_activeHeightmap->GetResolution();
    const Vector3 scale = m_activeHeightmap->GetWorldScale();
    const float extentX = resolution.x * scale.x;
    const float extentZ = resolution.y * scale.z;
    return std::max(extentX, extentZ);
}

Vector3 Renderer::GetFogColor() const {
    const Vector3 baseSkyColor(0.38f, 0.48f, 0.62f);
    const Vector3 ambientContribution = m_directionalLight.ambient * 0.35f;
    Vector3 color = baseSkyColor * 0.65f + ambientContribution;
    color.x = std::clamp(color.x, 0.0f, 1.0f);
    color.y = std::clamp(color.y, 0.0f, 1.0f);
    color.z = std::clamp(color.z, 0.0f, 1.0f);
    return color;
}

float Renderer::GetFogDensity() const {
    const float targetFactor = 0.08f;
    const float effectiveFar = std::max(m_farPlane * 0.9f, 1.0f);
    const float exponent = -std::log(std::max(targetFactor, 1e-3f));
    return std::sqrt(exponent) / effectiveFar;
}

void Renderer::UpdateViewRangeFromTerrain() {
    const float extent = GetTerrainExtent();
    if (extent <= 0.0f) {
        m_farPlane = std::max(m_farPlane, 1500.0f);
        return;
    }
    constexpr float kFarPadding = 1.15f;
    constexpr float kMinFar = 1500.0f;
    m_farPlane = std::max(extent * kFarPadding, kMinFar);
    m_nearPlane = std::max(0.3f, std::min(0.8f, m_farPlane * 0.0002f));
}