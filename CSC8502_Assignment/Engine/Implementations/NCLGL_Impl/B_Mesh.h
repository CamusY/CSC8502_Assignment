/**
* @file   B_Mesh.h
 * @brief  (Day 2 Stub) 轨道 B (nclgl) 对 I_Mesh 接口的实现。
 *
 * @class  NCLGL_Impl::B_Mesh
 * @brief  轨道 B 的网格包装器，实现了 Engine::IAL::I_Mesh 接口。
 * 此类是对 nclgl::Mesh 类的具体包装 (Wrapper)。
 * 根据 V13 计划 (NFR-1)，CSC8502_Assignment 层的代码严禁
 * 直接包含或实例化此文件，而应通过 I_ResourceFactory 
 * 获取 I_Mesh 接口。
 *
 * @var    NCLGL_Impl::B_Mesh::m_nclglMesh
 * @brief  指向被包装的 nclgl::Mesh 实例的原始指针。
 * 此指针的生命周期管理是一个关键约束：B_Mesh 实例
 * **不拥有** (does not own) 此指针。
 * 该指针所指向的 nclgl::Mesh 实例的创建和销毁
 * 完全由 B_Factory (或其他资源管理器) 负责。
 * B_Mesh 的析构函数不得 delete 此指针。
 *
 * @fn     NCLGL_Impl::B_Mesh::B_Mesh(nclgl::Mesh* nclMesh)
 * @brief  B_Mesh 的构造函数。
 * @param  nclMesh 一个指向 nclgl::Mesh 实例的有效原始指针。
 * 调用者 (通常是 B_Factory) 必须确保此指针在
 * B_Mesh 实例的整个生命周期内保持有效。
 * 此指针不能为 nullptr。
 *
 * @fn     NCLGL_Impl::B_Mesh::~B_Mesh()
 * @brief  B_Mesh 的析构函数。
 * (Day 2 Stub) 这是一个空实现。
 * 根据生命周期约束，此析构函数**严禁**
 * delete m_nclglMesh。
 *
 * @fn     NCLGL_Impl::B_Mesh::Draw()
 * @brief  (Day 2 Stub) 实现了 I_Mesh::Draw() 接口。
 * 此函数在 Day 2 阶段是一个空壳实现，不执行任何
 * OpenGL 绘制调用，以确保项目在核心架构
 * 搭建阶段可以成功链接和运行。
 * 在 Sprint 1 的 Day 7 (架构验证) 将被实现。
 */

#pragma once

#include "IAL/I_Mesh.h"

namespace nclgl
{
    class Mesh;
}

namespace NCLGL_Impl
{
    class B_Mesh : public Engine::IAL::I_Mesh
    {
    public:
        B_Mesh(nclgl::Mesh* nclMesh);
        virtual ~B_Mesh();

        virtual void Draw() override;

    private:
        nclgl::Mesh* m_nclglMesh;
    };
}