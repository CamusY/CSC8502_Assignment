/**
 * @file SceneManager.h
 * @brief 声明负责管理场景图与生命周期的 SceneManager 类。
 * @details
 * SceneManager 负责持有场景图实例、提供更新入口并协调资源工厂创建的场景内容。
 * 在 Day10 中它会实例化 Scene_T1_Peace，加载地形节点与贴图，并在 Update 中驱动场景逻辑。
 */
#pragma once

#include <memory>

#include "SceneGraph.h"

#include "IAL/I_ResourceFactory.h"

class Scene_T1_Peace;
class Scene_T2_War;
class Renderer;
class Water;

enum class SceneType {
    Peace,
    War
};

class SceneManager {
public:
    explicit SceneManager(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory);
    ~SceneManager();
    
    std::shared_ptr<SceneGraph> GetSceneGraph() const;

    void Update(float deltaTime);

    std::shared_ptr<Water> GetWater() const;
    SceneType GetActiveScene() const;
    bool SetActiveScene(SceneType type);
    void ApplyEnvironment(Renderer& renderer);

private:
    std::shared_ptr<Engine::IAL::I_ResourceFactory> m_factory;
    std::shared_ptr<SceneGraph> m_sceneGraph;
    float m_accumulatedTime;
    std::unique_ptr<Scene_T1_Peace> m_scenePeace;
    std::unique_ptr<Scene_T2_War> m_sceneWar;
    SceneType m_activeType;
};