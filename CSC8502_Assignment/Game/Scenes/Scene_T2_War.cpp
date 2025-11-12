/**
 * @file Scene_T2_War.cpp
 * @brief 实现 Day14 战争场景 (T2) 的地形与废墟加载逻辑。
 */
#include "Scene_T2_War.h"

#include "../../Engine/IAL/I_Heightmap.h"
#include "../../Engine/IAL/I_Texture.h"
#include "nclgl/Vector3.h"

Scene_T2_War::Scene_T2_War(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
                           const std::shared_ptr<SceneGraph>& sceneGraph)
    : m_factory(factory)
    , m_sceneGraph(sceneGraph)
    , m_terrainNode(nullptr)
    , m_ruinsNode(nullptr)
    , m_terrainTexture(nullptr)
    , m_environment{} {
}

Scene_T2_War::~Scene_T2_War() = default;

void Scene_T2_War::Init() {
    if (!m_factory || !m_sceneGraph) {
        return;
    }

    auto heightmap = m_factory->LoadHeightmap("../Heightmaps/terrain.png", Vector3(2.0f, 0.4f, 2.0f));
    if (!heightmap) {
        return;
    }

    m_terrainTexture = m_factory->LoadTexture("../Textures/terrain_war.png", false);

    m_terrainNode = std::make_shared<SceneNode>();
    m_terrainNode->SetMesh(heightmap);
    if (m_terrainTexture) {
        m_terrainNode->SetTexture(m_terrainTexture);
    }

    auto root = m_sceneGraph->GetRoot();
    if (root) {
        root->AddChild(m_terrainNode);
    }

    auto ruinsMesh = m_factory->LoadMesh("../Meshes/ruins.gltf");
    if (ruinsMesh) {
        m_ruinsNode = std::make_shared<SceneNode>();
        m_ruinsNode->SetMesh(ruinsMesh);
        m_ruinsNode->SetScale(Vector3(50.0f, 50.0f, 50.0f));
        m_ruinsNode->SetPosition(Vector3(512.0f, 15.0f, 512.0f));
        if (root) {
            root->AddChild(m_ruinsNode);
        }
    }

    m_environment.skyboxTexture = m_factory->LoadCubemap(
        "../Textures/skybox_war/negx.png",
        "../Textures/skybox_war/posx.png",
        "../Textures/skybox_war/negy.png",
        "../Textures/skybox_war/posy.png",
        "../Textures/skybox_war/negz.png",
        "../Textures/skybox_war/posz.png");
    m_environment.directionalLight.position = Vector3(150.0f, 300.0f, 150.0f);
    m_environment.directionalLight.color = Vector3(0.8f, 0.4f, 0.35f);
    m_environment.directionalLight.ambient = Vector3(0.1f, 0.08f, 0.1f);
    m_environment.sceneColour = Vector3(0.35f, 0.12f, 0.1f);
}

void Scene_T2_War::Update(float deltaTime) {
    (void)deltaTime;
}

std::shared_ptr<Water> Scene_T2_War::GetWater() const {
    return nullptr;
}

const SceneEnvironment& Scene_T2_War::GetEnvironment() const {
    return m_environment;
}

void Scene_T2_War::SetActive(bool active) {
    if (m_terrainNode) {
        m_terrainNode->SetActive(active);
    }
    if (m_ruinsNode) {
        m_ruinsNode->SetActive(active);
    }
}