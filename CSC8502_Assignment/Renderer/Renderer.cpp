/**
 * @file Renderer.cpp
 * @brief 实现基础渲染器的渲染队列遍历逻辑。
 * @details
 * 当前实现负责从场景图收集所有可渲染节点，并依次调用其网格接口执行 Draw。
 * 后续迭代将基于此骨架扩展材质、阴影、后处理等高级渲染流程。
 */
#include "Renderer.h"

#include "../Engine/IAL/I_Mesh.h"

Renderer::Renderer(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
                   const std::shared_ptr<SceneGraph>& sceneGraph)
    : m_factory(factory), m_sceneGraph(sceneGraph) {
}

void Renderer::Render() {
    if (!m_sceneGraph) {
        return;
    }
    m_renderQueue.clear();
    m_sceneGraph->CollectRenderableNodes(m_renderQueue);
    for (const auto& node : m_renderQueue) {
        if (node) {
            auto mesh = node->GetMesh();
            if (mesh) {
                mesh->Draw();
            }
        }
    }
}