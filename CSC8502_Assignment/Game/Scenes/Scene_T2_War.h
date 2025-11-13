/**
* @file Scene_T2_War.h
 * @brief 声明 Day14 战争场景 (T2) 的封装类。
 * @details
 * Scene_T2_War 负责加载废墟环境、夜色天空盒与焚毁地表贴图，并将其注入到 SceneGraph 中。
 */
#pragma once

#include <memory>

#include "../../Core/SceneGraph.h"
#include "../../Engine/IAL/I_ResourceFactory.h"
#include "../SceneEnvironment.h"

class Water;

class Scene_T2_War {
public:
    Scene_T2_War(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
                 const std::shared_ptr<SceneGraph>& sceneGraph);
    ~Scene_T2_War();

    void Init();
    void Update(float deltaTime);

    std::shared_ptr<Water> GetWater() const;
    const SceneEnvironment& GetEnvironment() const;
    void SetActive(bool active);

private:
    std::shared_ptr<Engine::IAL::I_ResourceFactory> m_factory;
    std::shared_ptr<SceneGraph> m_sceneGraph;
    std::shared_ptr<SceneNode> m_terrainNode;
    std::shared_ptr<SceneNode> m_ruinsNode;
    std::shared_ptr<Engine::IAL::I_Texture> m_terrainTexture;
    std::shared_ptr<Engine::IAL::I_AnimatedMesh> m_characterMesh;
    std::shared_ptr<Water> m_water;
    SceneEnvironment m_environment;
};