/**
* @file I_Mesh.h
 * @brief 定义了可渲染网格（Mesh）的基础抽象接口。
 * @details
 * 该文件的设计目的是为核心渲染器（Renderer）提供一个统一的、可绘制对象的基类。
 * 渲染器将通过此接口调用 Draw()，而无需知道它是一个静态网格（nclgl::Mesh）、
 * 高度图（I_Heightmap）还是骨骼动画网格（I_AnimatedMesh）。
 *
 * (NFR-1) 规范约束：本文件严禁包含 nclgl/Mesh.h。
 *
 * @class Engine::IAL::I_Mesh
 * @brief 可渲染网格的纯虚基类接口。
 * @details
 * 抽象了 nclgl::Mesh 的核心绘制功能。
 * 实例由 I_ResourceFactory 创建（例如 LoadMesh, LoadHeightmap 等）。
 *
 * @fn Engine::IAL::I_Mesh::~I_Mesh
 * @brief 虚拟析构函数。
 *
 * @fn Engine::IAL::I_Mesh::Draw
 * @brief 执行此网格的 GPU 绘制调用。
 * @details
 * 渲染器在遍历场景图（SceneGraph）时调用此函数。
 * 适配器实现（如 B_Mesh）将调用其内部持有的 nclgl::Mesh::Draw()。
 */

#pragma once

#include <memory>

namespace Engine::IAL {
    class I_Texture;

    class I_Mesh {
    public:
        virtual ~I_Mesh() {}
        virtual void Draw() = 0;

        virtual std::shared_ptr<I_Texture> GetDefaultTexture() const {
            return nullptr;
        }
    };

}
