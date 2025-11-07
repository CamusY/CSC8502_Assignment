/**
* @file B_Mesh.h
 * @brief 轨道 B (NCLGL_Impl) 的网格接口实现。
 *
 * 本文件定义了 B_Mesh 类，它是 Engine::IAL::I_Mesh 接口在 nclgl 框架下的具体适配器实现。
 * 它负责包装原生的 nclgl::Mesh 对象，使其能够被渲染引擎以统一的接口进行调用。
 *
 * B_Mesh 类 (NCLGL_Impl::B_Mesh):
 * 继承自 Engine::IAL::I_Mesh 纯虚接口。
 * 作为一个适配器 (Wrapper/Adapter)，它内部持有一个指向 nclgl::Mesh 的原生裸指针。
 * 它拥有该指针的所有权，负责在析构时释放该 nclgl::Mesh 资源。
 *
 * 构造函数 B_Mesh(::Mesh* mesh):
 * 接收一个由 nclgl 库创建的 Mesh 原生指针。
 * 参数 mesh: 指向有效 nclgl::Mesh 对象的指针。B_Mesh 将接管此指针的生命周期。
 * 如果传入 nullptr，后续的 Draw 调用可能会导致未定义行为（虽然在 Day 2 的空壳实现中是安全的）。
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
 * 类型为 ::Mesh* (全局命名空间下的 nclgl Mesh 类)。
 * 这是被适配的实际渲染对象。
 */
#pragma once
#include "IAL/I_Mesh.h"

class Mesh;

namespace NCLGL_Impl {

    class B_Mesh : public virtual Engine::IAL::I_Mesh {
    public:
        explicit B_Mesh(::Mesh* mesh);
        ~B_Mesh() override;

        void Draw() override;

    private:
        ::Mesh* m_mesh;
    };

}