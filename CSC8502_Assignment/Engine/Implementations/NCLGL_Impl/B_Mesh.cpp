/**
* @file   B_Mesh.cpp
 * @brief  (Day 2 Stub) B_Mesh 类的实现空壳。
 *
 * @see    B_Mesh.h (获取所有成员函数和变量的详细语义)
 *
 * @fn     NCLGL_Impl::B_Mesh::B_Mesh(nclgl::Mesh* nclMesh)
 * @brief  (Day 2 Stub) 构造函数实现。
 * 仅将传入的 nclgl::Mesh 指针赋值给成员变量 m_nclglMesh。
 *
 * @fn     NCLGL_Impl::B_Mesh::~B_Mesh()
 * @brief  (Day 2 Stub) 析构函数实现。
 * 空实现。严格遵守 B_Mesh.h 中定义的生命周期约束，
 * 不释放 m_nclglMesh。
 *
 * @fn     NCLGL_Impl::B_Mesh::Draw()
 * @brief  (Day 2 Stub) Draw() 接口的空壳实现。
 * 不执行任何操作。
 */

#include "B_Mesh.h"
#include "nclgl/Mesh.h"

namespace NCLGL_Impl
{
    B_Mesh::B_Mesh(nclgl::Mesh* nclMesh) : m_nclglMesh(nclMesh)
    {
    }

    B_Mesh::~B_Mesh()
    {
    }

    void B_Mesh::Draw()
    {
    }
}