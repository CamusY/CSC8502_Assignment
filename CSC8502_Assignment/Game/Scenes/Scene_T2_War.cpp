/**
 * @file Scene_T2_War.cpp
 * @brief 实现 Day14 战争场景 (T2) 的地形与废墟加载逻辑。
 */
#include "Scene_T2_War.h"

#include "../../Engine/IAL/I_Heightmap.h"
#include "../../Engine/IAL/I_Texture.h"
#include "../../Engine/IAL/I_AnimatedMesh.h"
#include "../../Renderer/Water.h"

#include "nclgl/Vector2.h"
#include "nclgl/Vector3.h"

Scene_T2_War::Scene_T2_War(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
                           const std::shared_ptr<SceneGraph>& sceneGraph) :
    m_factory(factory)
    , m_sceneGraph(sceneGraph)
    , m_terrainNode(nullptr)
    , m_ruinsNode(nullptr)
, m_lightFixtureNode(nullptr)
    , m_terrainTexture(nullptr)
    , m_environment{} {
}

Scene_T2_War::~Scene_T2_War() = default;

void Scene_T2_War::Init() {
    if (!m_factory || !m_sceneGraph) {
        return;
    }
    m_environment.pointLights.clear();

    m_heightmap = m_factory->LoadHeightmap("../Heightmaps/terrain.png", Vector3(2.0f, 0.4f, 2.0f));
    if (!m_heightmap) {
        return;
    }

    m_terrainTexture = m_factory->LoadTexture("../Textures/terrain_war.png", false);

    if (!m_terrainTexture) {
        m_terrainTexture = m_factory->LoadTexture("../Textures/terrain_texture.png", false);
    }
    m_water = std::make_shared<Water>(m_factory, 30.0f, Vector2(1024.0f, 1024.0f));
    m_terrainNode = std::make_shared<SceneNode>();
    m_terrainNode->SetMesh(m_heightmap);
    if (m_terrainTexture) {
        m_terrainNode->SetTexture(m_terrainTexture);
    }

    auto root = m_sceneGraph->GetRoot();
    if (root) {
        root->AddChild(m_terrainNode);
    }
    if (m_water) {
        auto waterNode = m_water->GetNode();
        if (root && waterNode) {
            root->AddChild(waterNode);
        }
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
    auto lightMesh = m_factory->LoadMesh("../Meshes/light.gltf");
    if (lightMesh) {
        m_lightFixtureNode = std::make_shared<SceneNode>();
        m_lightFixtureNode->SetMesh(lightMesh);
        m_lightFixtureNode->SetScale(Vector3(8.0f, 8.0f, 8.0f));
        Vector3 fixturePosition(360.0f, 65.0f, 512.0f);
        m_lightFixtureNode->SetPosition(fixturePosition);
        if (root) {
            root->AddChild(m_lightFixtureNode);
        }
        Light pointLight{};
        pointLight.position = fixturePosition + Vector3(0.0f, 10.0f, 0.0f);
        pointLight.color = Vector3(5.5f, 4.2f, 2.6f);
        pointLight.ambient = Vector3(12.25f, 12.18f, 12.12f);
        m_environment.pointLights.push_back(pointLight);
    }
    if (m_factory) {
        m_environment.skyboxTexture = m_factory->LoadCubemap(
            "../Textures/skybox_war/negx.jpg",
            "../Textures/skybox_war/posx.jpg",
            "../Textures/skybox_war/negy.jpg",
            "../Textures/skybox_war/posy.jpg",
            "../Textures/skybox_war/negz.jpg",
            "../Textures/skybox_war/posz.jpg");
        m_environment.grassBaseColorTexture = m_factory->LoadTexture("../Textures/grass/brownGrass.png", false);
    }
    m_environment.directionalLight.position = Vector3(150.0f, 300.0f, 150.0f);
    m_environment.directionalLight.color = Vector3(1.0f, 0.84f, 0.95f);
    m_environment.directionalLight.ambient = Vector3(0.21f, 0.28f, 0.31f);
    m_environment.sceneColour = Vector3(0.35f, 0.12f, 0.1f);
}

void Scene_T2_War::Update(float deltaTime) {
    (void)deltaTime;
}

std::shared_ptr<Water> Scene_T2_War::GetWater() const {
    return m_water;
}

std::shared_ptr<Engine::IAL::I_Heightmap> Scene_T2_War::GetHeightmap() const {
    return m_heightmap;
}

const SceneEnvironment& Scene_T2_War::GetEnvironment() const {
    return m_environment;
}

void Scene_T2_War::SetActive(bool active) {
    if (m_terrainNode) {
        m_terrainNode->SetActive(active);
    }
    if (m_water) {
        auto node = m_water->GetNode();
        if (node) {
            node->SetActive(active);
        }
    }
    if (m_ruinsNode) {
        m_ruinsNode->SetActive(active);
    }
    if (m_lightFixtureNode) {
        m_lightFixtureNode->SetActive(active);
    }
}
