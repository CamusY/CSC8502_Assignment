/**
 * @file Water.cpp
 * @brief 实现封装水体节点与尺寸信息的 Water 类。
 */
#include "Water.h"

Water::Water(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
             float height,
             const Vector2& size)
    : m_factory(factory)
    , m_node(nullptr)
    , m_height(height)
    , m_size(size) {
    if (!m_factory) {
        return;
    }
    m_node = std::make_shared<SceneNode>();
    if (!m_node) {
        return;
    }
    m_node->SetMesh(m_factory->CreateQuad());
    ConfigureNode(size);
}

Water::~Water() = default;

std::shared_ptr<SceneNode> Water::GetNode() const {
    return m_node;
}

float Water::GetHeight() const {
    return m_height;
}

Vector2 Water::GetSize() const {
    return m_size;
}

void Water::ConfigureNode(const Vector2& size) {
    if (!m_node) {
        return;
    }
    const float halfWidth = size.x * 0.5f;
    const float halfDepth = size.y * 0.5f;
    m_node->SetPosition(Vector3(halfWidth, m_height, halfDepth));
    m_node->SetScale(Vector3(halfWidth, 1.0f, halfDepth));
    m_node->SetRotation(Vector3(90.0f, 0.0f, 0.0f));
}