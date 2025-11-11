/**
* @file Renderer.cpp
 * @brief 实现基础渲染器的渲染队列遍历逻辑。
 * @details
 * 当前实现负责从场景图收集所有可渲染节点，并依次调用其网格接口执行 Draw。
 * 后续迭代将基于此骨架扩展材质、阴影、后处理等高级渲染流程。
 */
#include "Renderer.h"

#include "PostProcessing.h"
#include "../Engine/IAL/I_Mesh.h"
#include "../Engine/IAL/I_Shader.h"
#include "../Engine/IAL/I_Texture.h"

#include <glad/glad.h>
#include <iostream>


Renderer::Renderer(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
                   const std::shared_ptr<SceneGraph>& sceneGraph,
                   int width,
                   int height)
    : m_factory(factory)
    , m_sceneGraph(sceneGraph)
    , m_sceneColour(Vector3(0.8f, 0.45f, 0.25f))
    , m_surfaceWidth(width)
    , m_surfaceHeight(height) {
    if (m_factory) {
        m_sceneShader = m_factory->CreateShader("Shared/basic.vert", "Shared/basic.frag");
        m_postShader = m_factory->CreateShader("Shared/postprocess.vert", "Shared/postprocess.frag");
        m_postProcessing = std::make_shared<PostProcessing>(m_factory, width, height);
    }
}

void Renderer::Render() {
    if (!m_sceneGraph) {
        return;
    }
    if (m_postProcessing) {
        m_postProcessing->BeginCapture();
    }

    glEnable(GL_DEPTH_TEST);

    m_renderQueue.clear();
    m_sceneGraph->CollectRenderableNodes(m_renderQueue);

    Matrix4 view = Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 3.5f), Vector3(0.0f, 0.0f, 0.0f));
    const float aspect = m_surfaceHeight > 0 ? static_cast<float>(m_surfaceWidth) / static_cast<float>(m_surfaceHeight) : 1.0f;
    Matrix4 projection = Matrix4::Perspective(0.1f, 100.0f, aspect, 45.0f);
    Matrix4 viewProj = projection * view;

    if (m_sceneShader) {
        m_sceneShader->Bind();
        m_sceneShader->SetUniform("uViewProj", viewProj);
        m_sceneShader->SetUniform("uColor", m_sceneColour);

        for (const auto& node : m_renderQueue) {
            if (!node) {
                continue;
            }
            auto mesh = node->GetMesh();
            if (!mesh) {
                continue;
            }
            m_sceneShader->SetUniform("uModel", node->GetWorldTransform());
            mesh->Draw();
        }

        m_sceneShader->Unbind();
    }

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
}