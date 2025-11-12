/**
 * @file SceneGraph.cpp
 * @brief 实现场景图节点与整体树结构的逻辑。
 * @details
 * 提供 SceneNode 与 SceneGraph 的具体行为实现，包括节点的变换计算、
 * 父子关系维护、整棵树的世界矩阵更新以及可渲染节点的收集。
 */
#include "SceneGraph.h"

SceneNode::SceneNode()
    : m_mesh(nullptr),
      m_position(Vector3(0.0f, 0.0f, 0.0f)),
      m_scale(Vector3(1.0f, 1.0f, 1.0f)),
      m_rotation(Vector3(0.0f, 0.0f, 0.0f)),
      m_active(true) {
    m_worldTransform.ToIdentity();
}

void SceneNode::SetMesh(const std::shared_ptr<Engine::IAL::I_Mesh>& mesh) {
    m_mesh = mesh;
}

std::shared_ptr<Engine::IAL::I_Mesh> SceneNode::GetMesh() const {
    return m_mesh;
}

void SceneNode::SetPosition(const Vector3& position) {
    m_position = position;
}

const Vector3& SceneNode::GetPosition() const {
    return m_position;
}

void SceneNode::SetScale(const Vector3& scale) {
    m_scale = scale;
}

const Vector3& SceneNode::GetScale() const {
    return m_scale;
}

void SceneNode::SetRotation(const Vector3& rotationDegrees) {
    m_rotation = rotationDegrees;
}

const Vector3& SceneNode::GetRotation() const {
    return m_rotation;
}

void SceneNode::SetTexture(const std::shared_ptr<Engine::IAL::I_Texture>& texture) {
    m_texture = texture;
}

std::shared_ptr<Engine::IAL::I_Texture> SceneNode::GetTexture() const {
    return m_texture;
}


void SceneNode::SetActive(bool active) {
    m_active = active;
}

bool SceneNode::IsActive() const {
    return m_active;
}

const Matrix4& SceneNode::GetWorldTransform() const {
    return m_worldTransform;
}

void SceneNode::AddChild(const std::shared_ptr<SceneNode>& child) {
    if (!child) {
        return;
    }
    child->m_parent = weak_from_this();
    m_children.emplace_back(child);
}

void SceneNode::RemoveChild(const std::shared_ptr<SceneNode>& child) {
    if (!child) {
        return;
    }
    auto iterator = std::remove(m_children.begin(), m_children.end(), child);
    if (iterator != m_children.end()) {
        child->m_parent.reset();
        m_children.erase(iterator, m_children.end());
    }
}

const std::vector<std::shared_ptr<SceneNode>>& SceneNode::GetChildren() const {
    return m_children;
}

void SceneNode::UpdateWorldTransform(const Matrix4& parentTransform) {
    m_worldTransform = parentTransform * BuildLocalTransform();
    for (const auto& child : m_children) {
        if (child) {
            child->UpdateWorldTransform(m_worldTransform);
        }
    }
}

Matrix4 SceneNode::BuildLocalTransform() const {
    Matrix4 translation = Matrix4::Translation(m_position);
    Matrix4 rotationX = Matrix4::Rotation(m_rotation.x, Vector3(1.0f, 0.0f, 0.0f));
    Matrix4 rotationY = Matrix4::Rotation(m_rotation.y, Vector3(0.0f, 1.0f, 0.0f));
    Matrix4 rotationZ = Matrix4::Rotation(m_rotation.z, Vector3(0.0f, 0.0f, 1.0f));
    Matrix4 scale = Matrix4::Scale(m_scale);
    Matrix4 rotation = rotationZ * rotationY * rotationX;
    return translation * rotation * scale;
}

SceneGraph::SceneGraph() {
    m_root = std::make_shared<SceneNode>();
}

std::shared_ptr<SceneNode> SceneGraph::GetRoot() const {
    return m_root;
}

void SceneGraph::Update() {
    if (!m_root) {
        return;
    }
    Matrix4 identity;
    identity.ToIdentity();
    m_root->UpdateWorldTransform(identity);
}

void SceneGraph::CollectRenderableNodes(std::vector<std::shared_ptr<SceneNode>>& outNodes) const {
    outNodes.clear();
    CollectRenderableNodesRecursive(m_root, outNodes);
}

void SceneGraph::CollectRenderableNodesRecursive(const std::shared_ptr<SceneNode>& node,
                                                  std::vector<std::shared_ptr<SceneNode>>& outNodes) const {
    if (!node) {
        return;
    }
    if (node->IsActive() && node->GetMesh()) {
        outNodes.emplace_back(node);
    }
    for (const auto& child : node->GetChildren()) {
        if (child) {
            CollectRenderableNodesRecursive(child, outNodes);
        }
    }
}