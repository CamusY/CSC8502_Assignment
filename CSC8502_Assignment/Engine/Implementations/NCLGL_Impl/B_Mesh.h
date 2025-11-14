/**
 * @file B_Mesh.h
 * @brief 轨道 B (NCLGL_Impl) 的网格接口实现。
 *
 * 本文件定义了 B_Mesh 类，它是 Engine::IAL::I_Mesh 接口在 nclgl 框架下的具体适配器实现。
 * 它负责包装原生的 nclgl::Mesh 对象，使其能够被渲染引擎以统一的接口进行调用。
 *
 * B_Mesh 类 (NCLGL_Impl::B_Mesh):
 * 继承自 Engine::IAL::I_Mesh 纯虚接口。
 * 作为一个适配器 (Wrapper/Adapter)，它内部持有一个指向 nclgl::Mesh 的 shared_ptr，
 * 以便同时支持 GLTFLoader 返回的共享网格和传统 new Mesh 的所有权语义。
 *
 * 构造函数 B_Mesh(std::shared_ptr<::Mesh> mesh):
 * 接收一个共享网格指针，常用于 GLTFLoader 返回的共享实例。
 * 构造函数 B_Mesh(::Mesh* mesh):
 * 接收一个由 nclgl 库创建的 Mesh 原生指针，并自动封装为 shared_ptr 管理。
 *
 * 析构函数 ~B_Mesh():
 * 负责 delete 内部持有的 m_mesh 指针，防止内存泄漏。
 *
 * 成员函数 Draw():
 * 实现 I_Mesh::Draw 接口。
 * 在完整实现中，它将调用底层的 m_mesh->Draw() 来执行实际的 OpenGL 绘制命令。
 * 在 Day 2 的空壳实现中，它不执行任何操作。
 *
 * 成员变量 m_mesh:
 * 类型为 std::shared_ptr<::Mesh>。
 * 这是被适配的实际渲染对象。
 */
#pragma once
#include "IAL/I_Mesh.h"

#include <memory>

class Mesh;

namespace NCLGL_Impl {

    class B_Mesh : public Engine::IAL::I_Mesh {
    public:
        explicit B_Mesh(std::shared_ptr<::Mesh> mesh);
        explicit B_Mesh(::Mesh* mesh);
        ~B_Mesh() override;

        void Draw() override;

        std::shared_ptr<Engine::IAL::I_Texture> GetDefaultTexture() const override;
        void SetDefaultTexture(const std::shared_ptr<Engine::IAL::I_Texture>& texture);

        void SetPBRMaterial(const Engine::IAL::PBRMaterial& material);

        const Engine::IAL::PBRMaterial* GetPBRMaterial() const override;

    private:
        std::shared_ptr<::Mesh> m_mesh;
        std::shared_ptr<Engine::IAL::I_Texture> m_defaultTexture;
        bool m_hasPBR = false;
        Engine::IAL::PBRMaterial m_pbrMaterial;
    };
}