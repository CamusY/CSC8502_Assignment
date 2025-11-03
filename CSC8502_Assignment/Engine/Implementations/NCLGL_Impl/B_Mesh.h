#pragma once

#include "IAL/I_Mesh.h" // 包含我们要实现的 IAL 接口
#include <memory>       // 用于 std::unique_ptr

// 1. (规范) 在头文件中使用前向声明，保持 "NCLGL_Impl" 头的纯净性
//    NFR-1 (系统解耦) 的精神也适用于实现层内部的头文件
class Mesh; 

namespace NCLGL_Impl {

    /**
     * @brief 轨道 B (nclgl) 静态网格实现
     * @details
     * - 实现了 Engine::IAL::I_Mesh 接口。
     * - 这是一个围绕 nclgl::Mesh 的轻量级包装器。
     * - V13 规范 (4.2)：此类的实例将由 B_Factory 创建（例如，通过调用 nclgl::Mesh::LoadFromMeshFile）
     * 并转移其所有权。
     */
    class B_Mesh : public virtual Engine::IAL::I_Mesh {
    public:
        /**
         * @brief 构造函数，获取 nclgl::Mesh 实例的所有权。
         * @param nclMesh B_Factory 通过 nclgl::Mesh::LoadFromMeshFile 创建的实例指针。
         */
        B_Mesh(Mesh* nclMesh);
        
        /**
         * @brief 析构函数
         * @details std::unique_ptr 将自动删除 m_nclMesh
         */
        virtual ~B_Mesh();

        // --- I_Mesh 接口实现 ---

        /**
         * @brief 绘制网格
         * @details 直接调用 nclgl::Mesh::Draw()
         */
        virtual void Draw() override;

    private:
        // 2. (规范) 使用智能指针管理被包装对象的生命周期
        std::unique_ptr<Mesh> m_nclMesh; 
    };

} // namespace NCLGL_Impl