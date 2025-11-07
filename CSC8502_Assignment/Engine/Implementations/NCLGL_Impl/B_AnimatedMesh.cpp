/**
* @file B_AnimatedMesh.cpp
 * @brief 轨道 B (NCLGL_Impl) 的骨骼动画网格接口实现源文件。
 *
 * 本文件实现了 B_AnimatedMesh 类。
 * 在 Day 2 阶段，所有函数均为基础的空壳实现 (Stubs)。
 * 析构函数已包含基础的资源释放逻辑。
 */
#include "B_AnimatedMesh.h"
#include "nclgl/Mesh.h"
#include "nclgl/MeshAnimation.h"

namespace NCLGL_Impl {

    B_AnimatedMesh::B_AnimatedMesh(::Mesh* mesh, ::MeshAnimation* anim)
        : m_mesh(mesh), m_anim(anim) {
    }

    B_AnimatedMesh::~B_AnimatedMesh() {
        delete m_mesh;
        delete m_anim;
    }

    void B_AnimatedMesh::Draw() {
    }

    void B_AnimatedMesh::UpdateAnimation(float dt) {
    }

    const std::vector<Matrix4>& B_AnimatedMesh::GetBoneTransforms() const {
        return m_boneTransforms;
    }

}