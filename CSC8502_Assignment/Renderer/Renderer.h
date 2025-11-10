/**
* @file Renderer.h
 * @brief 声明负责遍历场景图并提交绘制命令的 Renderer 类。
 * @details
 * Renderer 持有资源工厂引用以及场景图指针，通过 CollectRenderableNodes 收集节点，
 * 在 Render 函数中遍历并调用 I_Mesh::Draw()。本阶段实现的是基础渲染骨架，
 * 后续将扩展光照、后处理与多通道渲染。
 */
#pragma once

#include <memory>
#include <vector>

#include "../Core/SceneGraph.h"

#include "../Engine/IAL/I_ResourceFactory.h"

namespace Engine::IAL {
    class I_Shader;
}

class PostProcessing;

class Renderer {
public:
    Renderer(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
             const std::shared_ptr<SceneGraph>& sceneGraph,
             int width,
             int height);

    void Render();

private:
    std::shared_ptr<Engine::IAL::I_ResourceFactory> m_factory;
    std::shared_ptr<SceneGraph> m_sceneGraph;
    std::vector<std::shared_ptr<SceneNode>> m_renderQueue;
    std::shared_ptr<PostProcessing> m_postProcessing;
};