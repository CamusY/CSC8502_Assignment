/**
* @file B_Heightmap.cpp
 * @brief 轨道 B (NCLGL_Impl) 的高度图接口实现源文件。
 *
 * 本文件实现了 B_Heightmap 类。
 * 在 Day 2 阶段，它是一个空壳实现，析构函数包含基本的资源释放逻辑。
 */
#include "B_Heightmap.h"
#include "nclgl/Mesh.h"

namespace NCLGL_Impl {

    B_Heightmap::B_Heightmap(::Mesh* mesh) : m_mesh(mesh) {
    }

    B_Heightmap::~B_Heightmap() {
        delete m_mesh;
    }

    void B_Heightmap::Draw() {
    }

}