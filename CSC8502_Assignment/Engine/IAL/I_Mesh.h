#pragma once

// NFR-2: 接口直接依赖 nclgl 通用数学库
// (此文件不需要，但 I_AnimatedMesh 需要)

// 规范 4.1: 所有 IAL 接口均定义在 Engine::IAL 命名空间下
namespace Engine::IAL {

    /**
     * @brief IAL 静态网格接口 (P-0)
     * @details
     * - 抽象了 nclgl::Mesh。
     * - 这是 Demo 层（如 SceneGraph）所依赖的基础渲染对象。
     * - Draw: 抽象 nclgl::Mesh::Draw，用于在渲染器中发出绘制调用。
     */
    class I_Mesh {
    public:
        virtual ~I_Mesh() {}
        
        virtual void Draw() = 0;
    };

} // namespace Engine::IAL