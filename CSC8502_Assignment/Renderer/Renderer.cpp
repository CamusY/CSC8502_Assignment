/**
 * @file Renderer.cpp
 * @brief 实现基础渲染器的渲染队列遍历逻辑。
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

#include <glad/glad.h>
#include <iostream>




Renderer::Renderer(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
                   const std::shared_ptr<SceneGraph>& sceneGraph,
                   const std::shared_ptr<Camera>& camera,
                   const std::shared_ptr<Engine::IAL::I_DebugUI>& debugUI,
                   int width,
                   int height) :
    m_factory(factory)
    , m_sceneGraph(sceneGraph)
    , m_camera(camera)
    , m_directionalLight{}
    , m_sceneColour(Vector3(0.8f, 0.45f, 0.25f))
    , m_specularPower(32.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(5000.0f)
    , m_surfaceWidth(width)
    , m_surfaceHeight(height) {
    if (m_factory) {
        m_sceneShader = m_factory->CreateShader("Shared/basic.vert", "Shared/basic.frag");
        m_terrainShader = m_factory->CreateShader("Shared/terrain.vert", "Shared/terrain.frag");
        m_postShader = m_factory->CreateShader("Shared/postprocess.vert", "Shared/postprocess.frag");
        m_skyboxShader = m_factory->CreateShader("Shared/skybox.vert", "Shared/skybox.frag");
        m_waterShader = m_factory->CreateShader("Shared/water.vert", "Shared/water.frag");
        m_skyboxTexture = m_factory->LoadCubemap(
            "../Textures/skybox_peace/negx.png",
            "../Textures/skybox_peace/posx.png",
            "../Textures/skybox_peace/negy.png",
            "../Textures/skybox_peace/posy.png",
            "../Textures/skybox_peace/negz.png",
            "../Textures/skybox_peace/posz.png");
        m_skyboxMesh = m_factory->LoadMesh("../Meshes/cube.gltf");
        m_postProcessing = std::make_shared<PostProcessing>(m_factory, width, height);
    }

    m_directionalLight.position = Vector3(200.0f, 400.0f, 200.0f);
    m_directionalLight.color = Vector3(1.0f, 0.95f, 0.85f);
    m_directionalLight.ambient = Vector3(0.25f, 0.25f, 0.3f);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void Renderer::Render() {
    if (!m_sceneGraph) {
        RenderDebugUI();
        return;
    }

    glEnable(GL_DEPTH_TEST);

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
    Matrix4 projection = Matrix4::Perspective(m_nearPlane, m_farPlane, aspect, 45.0f);

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
        auto texture = m_postProcessing->GetSceneTexture();
        if (texture && m_postShader) {
            m_postShader->Bind();
            texture->Bind(0);
            m_postShader->SetUniform("uScene", 0);
            m_postProcessing->Present();
            m_postShader->Unbind();
        }
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
                               bool skipWaterNode) {
    if (!m_sceneGraph) {
        return;
    }
    m_renderQueue.clear();
    m_sceneGraph->CollectRenderableNodes(m_renderQueue);
    Matrix4 viewProj = projection * view;

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
        auto texture = node->GetTexture();
        std::shared_ptr<Engine::IAL::I_Shader> shader = texture ? m_terrainShader : m_sceneShader;
        if (!shader) {
            continue;
        }
        shader->Bind();
        shader->SetUniform("uViewProj", viewProj);
        shader->SetUniform("uModel", node->GetWorldTransform());
        if (texture) {
            texture->Bind(0);
            shader->SetUniform("uDiffuse", 0);
            shader->SetUniform("uLightPosition", m_directionalLight.position);
            shader->SetUniform("uLightColor", m_directionalLight.color);
            shader->SetUniform("uAmbientColor", m_directionalLight.ambient);
            shader->SetUniform("uSpecularPower", m_specularPower);
            shader->SetUniform("uCameraPos", cameraPosition);
        }
        else {
            shader->SetUniform("uColor", m_sceneColour);
        }
        mesh->Draw();
        shader->Unbind();
    }
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
    m_waterShader->SetUniform("uViewProj", viewProj);
    m_waterShader->SetUniform("uModel", waterNode->GetWorldTransform());
    m_waterShader->SetUniform("uCameraPos", cameraPosition);
    m_waterShader->SetUniform("uLightColor", m_directionalLight.color);
    m_waterShader->SetUniform("uAmbientColor", m_directionalLight.ambient);
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
    RenderSkybox(reflectionView, projection);
    RenderScenePass(reflectionView, projection, reflectionCamera.GetPosition(), true);

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
    RenderScenePass(view, projection, cameraPosition, true);

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