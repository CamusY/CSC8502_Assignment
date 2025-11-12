/**
 * @file SceneGraph.h
 * @brief 声明基于层次结构的场景图数据结构。
 * @details
 * 本文件定义了 SceneNode 与 SceneGraph 两个核心类，用于管理渲染场景中的层次结构。
 * SceneNode 表示一个在场景图中可被遍历的节点，提供位置、缩放、旋转及网格对象的维护接口。
 * SceneGraph 维护一个根节点并负责驱动整棵树的世界变换更新与可渲染节点的收集。
 *
 * SceneNode:
 *  - 使用 nclgl 数学库 (Matrix4, Vector3) 维护本地与世界空间的变换。
 *  - 通过 Engine::IAL::I_Mesh 接口引用可渲染对象，保证 Demo 层仅依赖纯净接口。
 *  - 采用 shared_ptr/weak_ptr 建模父子关系，提供对子节点的添加、移除与访问功能。
 *  - 提供 UpdateWorldTransform 接口以在遍历时同步世界矩阵。
 *
 * SceneGraph:
 *  - 在构造时创建一颗空的根节点作为场景的入口。
 *  - 提供 Update 方法以从根节点开始更新所有节点的世界矩阵。
 *  - 提供 CollectRenderableNodes 方法以深度优先收集所有拥有可绘制网格的节点，供渲染器使用。
 */
#pragma once

#include <memory>
#include <vector>
#include <algorithm>

#include "nclgl/Matrix4.h"
#include "nclgl/Vector3.h"

#include "IAL/I_Mesh.h"
#include "IAL/I_Texture.h"

class SceneNode : public std::enable_shared_from_this<SceneNode> {
public:
    SceneNode();

    void SetMesh(const std::shared_ptr<Engine::IAL::I_Mesh>& mesh);
    std::shared_ptr<Engine::IAL::I_Mesh> GetMesh() const;

    void SetPosition(const Vector3& position);
    const Vector3& GetPosition() const;

    void SetScale(const Vector3& scale);
    const Vector3& GetScale() const;

    void SetRotation(const Vector3& rotationDegrees);
    const Vector3& GetRotation() const;
    
    void SetTexture(const std::shared_ptr<Engine::IAL::I_Texture>& texture);
    std::shared_ptr<Engine::IAL::I_Texture> GetTexture() const;
    
    void SetActive(bool active);
    bool IsActive() const;

    const Matrix4& GetWorldTransform() const;

    void AddChild(const std::shared_ptr<SceneNode>& child);
    void RemoveChild(const std::shared_ptr<SceneNode>& child);
    const std::vector<std::shared_ptr<SceneNode>>& GetChildren() const;

    void UpdateWorldTransform(const Matrix4& parentTransform);

private:
    Matrix4 BuildLocalTransform() const;

    std::weak_ptr<SceneNode> m_parent;
    std::vector<std::shared_ptr<SceneNode>> m_children;

    std::shared_ptr<Engine::IAL::I_Mesh> m_mesh;
    std::shared_ptr<Engine::IAL::I_Texture> m_texture;

    Vector3 m_position;
    Vector3 m_scale;
    Vector3 m_rotation;

    Matrix4 m_worldTransform;
    bool m_active;
};

class SceneGraph {
public:
    SceneGraph();

    std::shared_ptr<SceneNode> GetRoot() const;

    void Update();

    void CollectRenderableNodes(std::vector<std::shared_ptr<SceneNode>>& outNodes) const;

private:
    void CollectRenderableNodesRecursive(const std::shared_ptr<SceneNode>& node,
                                         std::vector<std::shared_ptr<SceneNode>>& outNodes) const;

    std::shared_ptr<SceneNode> m_root;
};