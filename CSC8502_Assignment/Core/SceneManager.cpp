/**
 * @file SceneManager.cpp
 * @brief 实现场景管理器的核心逻辑。
 * @details
 * 构造函数接收资源工厂接口并创建场景图实例，Update 函数负责驱动场景图的更新。
 * 计时累积字段将在后续迭代中用于驱动动画或时间轴过渡。
 */
#include "SceneManager.h"

SceneManager::SceneManager(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory)
    : m_factory(factory), m_sceneGraph(std::make_shared<SceneGraph>()), m_accumulatedTime(0.0f) {
    if (m_sceneGraph && m_factory) {
        auto root = m_sceneGraph->GetRoot();
        if (root) {
            auto cubeNode = std::make_shared<SceneNode>();
            cubeNode->SetMesh(m_factory->LoadMesh("../Meshes/cube.gltf"));
            cubeNode->SetScale(Vector3(1.0f, 1.0f, 1.0f));
            root->AddChild(cubeNode);
        }
    }
}

std::shared_ptr<SceneGraph> SceneManager::GetSceneGraph() const {
    return m_sceneGraph;
}

void SceneManager::Update(float deltaTime) {
    m_accumulatedTime += deltaTime;
    if (m_sceneGraph) {
        m_sceneGraph->Update();
    }
}