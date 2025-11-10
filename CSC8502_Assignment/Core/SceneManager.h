/**
* @file SceneManager.h
 * @brief 声明负责管理场景图与生命周期的 SceneManager 类。
 * @details
 * SceneManager 负责持有场景图实例、提供更新入口并协调资源工厂创建的场景内容。(预留）
 * 它是 Application 与渲染层之间的纽带，未来会扩展以加载不同的场景、
 * 切换时间轴或触发过渡效果。本阶段提供最小化接口以驱动 SceneGraph 的更新。
 */
#pragma once

#include <memory>

#include "SceneGraph.h"

#include "IAL/I_ResourceFactory.h"

class SceneManager {
public:
    explicit SceneManager(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory);

    std::shared_ptr<SceneGraph> GetSceneGraph() const;

    void Update(float deltaTime);

private:
    std::shared_ptr<Engine::IAL::I_ResourceFactory> m_factory;
    std::shared_ptr<SceneGraph> m_sceneGraph;
    float m_accumulatedTime;
};