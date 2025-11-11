/**
* @file B_Mesh.cpp
 * @brief 轨道 B (NCLGL_Impl) 的网格接口实现源文件。
 *
 * 本文件实现了 B_Mesh 类。
 * 在 Day 2 阶段，所有函数均为基础的空壳实现 (Stubs)，仅用于验证架构的链接连通性。
 * 包含了对 nclgl/Mesh.h 的依赖，以便在未来完整实现中调用其原生方法。
 */
#include "B_Mesh.h"
#include "nclgl/Mesh.h"

namespace NCLGL_Impl {

    B_Mesh::B_Mesh(std::shared_ptr<::Mesh> mesh)
        : m_mesh(std::move(mesh)) {
    }

    B_Mesh::B_Mesh(::Mesh* mesh)
        : m_mesh(std::shared_ptr<::Mesh>(mesh)) {
    }

    B_Mesh::~B_Mesh() {
    }

    void B_Mesh::Draw() {
        if (m_mesh) {
            m_mesh->Draw();
        }
    }

}