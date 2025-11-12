/**
 * @file SceneManager.cpp
 * @brief 实现场景管理器的核心逻辑。
 * @details
 * 构造函数接收资源工厂接口并创建场景图实例，Update 函数负责驱动场景图的更新。
 * 计时累积字段将在后续迭代中用于驱动动画或时间轴过渡。
 */

#include "SceneManager.h"
#include "../Renderer/Water.h"
#include "../Game/Scenes/Scene_T1_Peace.h"

SceneManager::SceneManager(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory)
    : m_factory(factory)
    , m_sceneGraph(std::make_shared<SceneGraph>())
    , m_accumulatedTime(0.0f)
    , m_activeScene(nullptr) {
    if (m_factory && m_sceneGraph) {
        m_activeScene = std::make_unique<Scene_T1_Peace>(m_factory, m_sceneGraph);
        if (m_activeScene) {
            m_activeScene->Init();
        }
    }
}

std::shared_ptr<SceneGraph> SceneManager::GetSceneGraph() const {
    return m_sceneGraph;
}

void SceneManager::Update(float deltaTime) {
    m_accumulatedTime += deltaTime;
    if (m_activeScene) {
        m_activeScene->Update(deltaTime);
    }
    if (m_sceneGraph) {
        m_sceneGraph->Update();
    }
}

SceneManager::~SceneManager() = default;

std::shared_ptr<Water> SceneManager::GetWater() const {
    if (!m_activeScene) {
        return nullptr;
    }
    return m_activeScene->GetWater();
}