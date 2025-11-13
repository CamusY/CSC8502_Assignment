/**
* @file Scene_T1_Peace.cpp
 * @brief 实现 Day10 和平场景 (T1) 的地形加载与绑定逻辑。
 */
#include "Scene_T1_Peace.h"

#include "../../Engine/IAL/I_Heightmap.h"
#include "../../Engine/IAL/I_Texture.h"
#include "../../Engine/IAL/I_AnimatedMesh.h"
#include "../../Renderer/Water.h"
#include "../../Core/TerrainConfig.h"
#include "nclgl/Vector2.h"
#include "nclgl/Vector3.h"

Scene_T1_Peace::Scene_T1_Peace(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
                               const std::shared_ptr<SceneGraph>& sceneGraph) :
    m_factory(factory)
    , m_sceneGraph(sceneGraph)
    , m_terrainNode(nullptr)
    , m_terrainTexture(nullptr)
    , m_characterNode(nullptr)
    , m_characterMesh(nullptr)
    , m_environment{} {
}

Scene_T1_Peace::~Scene_T1_Peace() = default;

void Scene_T1_Peace::Init() {
    if (!m_factory || !m_sceneGraph) {
        return;
    }

    auto heightmap = m_factory->LoadHeightmap("../Heightmaps/terrain.png", kTerrainScale);
    if (!heightmap) {
        return;
    }

    m_terrainTexture = m_factory->LoadTexture("../Textures/terrain_texture.png", false);

    if (!m_terrainTexture) {
        m_terrainTexture = m_factory->LoadTexture("../Heightmaps/terrain.png", false);
    }

    m_terrainNode = std::make_shared<SceneNode>();
    m_terrainNode->SetMesh(heightmap);
    if (m_terrainTexture) {
        m_terrainNode->SetTexture(m_terrainTexture);
    }

    auto root = m_sceneGraph->GetRoot();
    if (root) {
        root->AddChild(m_terrainNode);
    }

    const Vector2 waterSize(kTerrainExtent, kTerrainExtent);
    m_water = std::make_shared<Water>(m_factory, 30.0f, waterSize);
    if (m_water) {
        auto waterNode = m_water->GetNode();
        if (root && waterNode) {
            root->AddChild(waterNode);
        }
    }

    if (m_factory) {
        m_environment.skyboxTexture = m_factory->LoadCubemap(
            "../Textures/skybox_peace/negx.png",
            "../Textures/skybox_peace/posx.png",
            "../Textures/skybox_peace/negy.png",
            "../Textures/skybox_peace/posy.png",
            "../Textures/skybox_peace/negz.png",
            "../Textures/skybox_peace/posz.png");

        m_characterMesh = m_factory->LoadAnimatedMesh("../Meshes/CesiumMan/CesiumMan.gltf");
        if (m_characterMesh) {
            m_characterNode = std::make_shared<SceneNode>();
            m_characterNode->SetMesh(m_characterMesh);
            m_characterNode->SetScale(Vector3(80.0f, 80.0f, 80.0f));
            m_characterNode->SetPosition(Vector3(1040.0f, 15.0f, 1000.0f));
            if (root) {
                root->AddChild(m_characterNode);
            }
        }
    }
    m_environment.directionalLight.position = Vector3(200.0f, 400.0f, 200.0f);
    m_environment.directionalLight.color = Vector3(1.0f, 0.95f, 0.85f);
    m_environment.directionalLight.ambient = Vector3(0.25f, 0.25f, 0.3f);
    m_environment.sceneColour = Vector3(0.8f, 0.45f, 0.25f);
}

void Scene_T1_Peace::Update(float deltaTime) {
    (void)deltaTime;
}

std::shared_ptr<Water> Scene_T1_Peace::GetWater() const {
    return m_water;
}

const SceneEnvironment& Scene_T1_Peace::GetEnvironment() const {
    return m_environment;
}

void Scene_T1_Peace::SetActive(bool active) {
    if (m_terrainNode) {
        m_terrainNode->SetActive(active);
    }
    if (m_water) {
        auto node = m_water->GetNode();
        if (node) {
            node->SetActive(active);
        }
    }
    if (m_characterNode) {
        m_characterNode->SetActive(active);
    }
}
