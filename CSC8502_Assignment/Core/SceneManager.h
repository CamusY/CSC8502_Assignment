/**
* @file SceneManager.h
 * @brief 声明负责管理场景图与生命周期的 SceneManager 类。
 * @details
 * SceneManager 负责持有场景图实例、提供更新入口并协调资源工厂创建的场景内容。
 * Day15 起它还承担过渡计时的调度：当检测到过渡键时，会切换场景节点状态、
 * 通知 Renderer 更新环境并向 PostProcessing 推送过渡进度。
 */
#pragma once

#include <memory>

#include "SceneGraph.h"

#include "IAL/I_ResourceFactory.h"

namespace Engine::IAL {
    class I_Keyboard;
}

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

    void BindRenderer(const std::shared_ptr<Renderer>& renderer);
    void Update(float deltaTime,Engine::IAL::I_Keyboard* keyboard);

    std::shared_ptr<Water> GetWater() const;
    SceneType GetActiveScene() const;
    bool SetActiveScene(SceneType type);
    void ApplyEnvironment(Renderer& renderer);

private:
    void BeginTransition(SceneType target);

    std::shared_ptr<Engine::IAL::I_ResourceFactory> m_factory;
    std::shared_ptr<SceneGraph> m_sceneGraph;
    std::weak_ptr<Renderer> m_renderer;
    float m_accumulatedTime;
    std::unique_ptr<Scene_T1_Peace> m_scenePeace;
    std::unique_ptr<Scene_T2_War> m_sceneWar;
    SceneType m_activeType;
    bool m_transitionActive;
    float m_transitionElapsed;
    float m_transitionDuration;
};