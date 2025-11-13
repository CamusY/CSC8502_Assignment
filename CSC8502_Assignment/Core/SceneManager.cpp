/**
* @file SceneManager.cpp
 * @brief 实现场景管理器的核心逻辑。
 * @details
 * 构造函数接收资源工厂接口并创建场景图实例，Update 函数负责驱动场景图的更新。
 * Day15 起会在检测到过渡键时启动计时器，并向 Renderer 推送过渡进度以驱动全屏特效。
 */

#include "SceneManager.h"
#include "../Renderer/Water.h"
#include "../Renderer/Renderer.h"
#include "../Game/Scenes/Scene_T1_Peace.h"
#include "../Game/Scenes/Scene_T2_War.h"
#include "IAL/I_InputDevice.h"

#include <algorithm>


SceneManager::SceneManager(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory)
    : m_factory(factory)
    , m_sceneGraph(std::make_shared<SceneGraph>())
    , m_renderer()
    , m_accumulatedTime(0.0f)
    , m_scenePeace(nullptr)
    , m_sceneWar(nullptr)
    , m_activeType(SceneType::Peace)
    , m_transitionActive(false)
    , m_transitionElapsed(0.0f)
    , m_transitionDuration(1.0f) {
    if (m_factory && m_sceneGraph) {
        m_scenePeace = std::make_unique<Scene_T1_Peace>(m_factory, m_sceneGraph);
        if (m_scenePeace) {
            m_scenePeace->Init();
            m_scenePeace->SetActive(true);
        }
        m_sceneWar = std::make_unique<Scene_T2_War>(m_factory, m_sceneGraph);
        if (m_sceneWar) {
            m_sceneWar->Init();
            m_sceneWar->SetActive(false);
        }
    }
}

std::shared_ptr<SceneGraph> SceneManager::GetSceneGraph() const {
    return m_sceneGraph;
}

void SceneManager::BindRenderer(const std::shared_ptr<Renderer>& renderer) {
    m_renderer = renderer;
    if (renderer) {
        ApplyEnvironment(*renderer);
        renderer->SetWater(GetWater());
    }
}

void SceneManager::Update(float deltaTime,Engine::IAL::I_Keyboard* keyboard) {
    m_accumulatedTime += deltaTime;

    if (keyboard && !m_transitionActive) {
        if (keyboard->KeyTriggered(Engine::IAL::KeyCode::K1)) {
            BeginTransition(SceneType::Peace);
        }
        else if (keyboard->KeyTriggered(Engine::IAL::KeyCode::K2)) {
            BeginTransition(SceneType::War);
        }
    }

    if (m_transitionActive) {
        m_transitionElapsed += deltaTime;
        const float progress = m_transitionDuration > 0.0f
            ? std::clamp(m_transitionElapsed / m_transitionDuration, 0.0f, 1.0f)
            : 1.0f;
        if (auto renderer = m_renderer.lock()) {
            renderer->SetTransitionState(true, progress);
        }
        if (m_transitionElapsed >= m_transitionDuration) {
            m_transitionActive = false;
            if (auto renderer = m_renderer.lock()) {
                renderer->SetTransitionState(false, 0.0f);
            }
        }
    }

    if (m_activeType == SceneType::Peace) {
        if (m_scenePeace) {
            m_scenePeace->Update(deltaTime);
        }
    }
    else if (m_activeType == SceneType::War) {
        if (m_sceneWar) {
            m_sceneWar->Update(deltaTime);
        }
    }
    if (m_sceneGraph) {
        m_sceneGraph->Update();
    }
}

SceneManager::~SceneManager() = default;

std::shared_ptr<Water> SceneManager::GetWater() const {
    if (m_activeType == SceneType::Peace && m_scenePeace) {
        return m_scenePeace->GetWater();
    }
    if (m_activeType == SceneType::War && m_sceneWar) {
        return m_sceneWar->GetWater();
    }
    return nullptr;
}

SceneType SceneManager::GetActiveScene() const {
    return m_activeType;
}

bool SceneManager::SetActiveScene(SceneType type) {
    if (type == m_activeType) {
        return false;
    }
    if (type == SceneType::Peace) {
        if (m_scenePeace) {
            m_scenePeace->SetActive(true);
        }
        if (m_sceneWar) {
            m_sceneWar->SetActive(false);
        }
    }
    else {
        if (m_scenePeace) {
            m_scenePeace->SetActive(false);
        }
        if (m_sceneWar) {
            m_sceneWar->SetActive(true);
        }
    }
    m_activeType = type;
    return true;
}

void SceneManager::ApplyEnvironment(Renderer& renderer) {
    const SceneEnvironment* environment = nullptr;
    if (m_activeType == SceneType::Peace && m_scenePeace) {
        environment = &m_scenePeace->GetEnvironment();
    }
    else if (m_activeType == SceneType::War && m_sceneWar) {
        environment = &m_sceneWar->GetEnvironment();
    }

    if (!environment) {
        Light fallback{};
        renderer.SetSkyboxTexture(nullptr);
        renderer.SetDirectionalLight(fallback);
        renderer.SetSceneColour(Vector3(0.0f, 0.0f, 0.0f));
        return;
    }

    renderer.SetSkyboxTexture(environment->skyboxTexture);
    renderer.SetDirectionalLight(environment->directionalLight);
    renderer.SetSceneColour(environment->sceneColour);
}

void SceneManager::BeginTransition(SceneType target) {
    if (!SetActiveScene(target)) {
        return;
    }
    m_transitionActive = true;
    m_transitionElapsed = 0.0f;
    if (auto renderer = m_renderer.lock()) {
        ApplyEnvironment(*renderer);
        renderer->SetWater(GetWater());
        renderer->SetTransitionState(true, 0.0f);
    }
}