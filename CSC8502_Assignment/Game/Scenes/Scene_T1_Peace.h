/**
* @file Scene_T1_Peace.h
 * @brief 声明负责构建 Day10 和平场景 (T1) 的封装类。
 *
 * Scene_T1_Peace 负责使用资源工厂加载 Day10 所需的地形高度图与贴图资源，
 * 并将其注入到 SceneGraph 中，提供后续渲染流程遍历的入口。
 */
#pragma once

#include <memory>

#include "../../Core/SceneGraph.h"
#include "../../Engine/IAL/I_ResourceFactory.h"
#include "../../Engine/IAL/I_AnimatedMesh.h"
#include "../SceneEnvironment.h"

class Water;

class Scene_T1_Peace {
public:
    Scene_T1_Peace(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
                   const std::shared_ptr<SceneGraph>& sceneGraph);
    ~Scene_T1_Peace();

    void Init();
    void Update(float deltaTime);

    std::shared_ptr<Water> GetWater() const;
    const SceneEnvironment& GetEnvironment() const;
    void SetActive(bool active);

private:
    std::shared_ptr<Engine::IAL::I_ResourceFactory> m_factory;
    std::shared_ptr<SceneGraph> m_sceneGraph;
    std::shared_ptr<SceneNode> m_terrainNode;
    std::shared_ptr<Engine::IAL::I_Texture> m_terrainTexture;
    std::shared_ptr<SceneNode> m_characterNode;
    std::shared_ptr<Engine::IAL::I_AnimatedMesh> m_characterMesh;
    std::shared_ptr<Water> m_water;
    SceneEnvironment m_environment;
};