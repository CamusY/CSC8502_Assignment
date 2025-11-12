/**
* @file Scene_T1_Peace.cpp
 * @brief 实现 Day10 和平场景 (T1) 的地形加载与绑定逻辑。
 */
#include "Scene_T1_Peace.h"

#include "../../Engine/IAL/I_Heightmap.h"
#include "../../Engine/IAL/I_Texture.h"
#include "nclgl/Vector3.h"

Scene_T1_Peace::Scene_T1_Peace(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
                               const std::shared_ptr<SceneGraph>& sceneGraph)
    : m_factory(factory)
    , m_sceneGraph(sceneGraph)
    , m_terrainNode(nullptr)
    , m_terrainTexture(nullptr) {
}

Scene_T1_Peace::~Scene_T1_Peace() = default;

void Scene_T1_Peace::Init() {
    if (!m_factory || !m_sceneGraph) {
        return;
    }

    auto heightmap = m_factory->LoadHeightmap("../Heightmaps/terrain.png", Vector3(2.0f, 0.4f, 2.0f));
    if (!heightmap) {
        return;
    }

    m_terrainTexture = m_factory->LoadTexture("../Textures/terrain_grass.png", false);

    m_terrainNode = std::make_shared<SceneNode>();
    m_terrainNode->SetMesh(heightmap);
    if (m_terrainTexture) {
        m_terrainNode->SetTexture(m_terrainTexture);
    }

    auto root = m_sceneGraph->GetRoot();
    if (root) {
        root->AddChild(m_terrainNode);
    }
}

void Scene_T1_Peace::Update(float deltaTime) {
    (void)deltaTime;
}