#pragma once

// 1. (规范) B_Heightmap 是 B_Mesh 的一种
#include "B_Mesh.h" 
// 2. (规范) 实现 I_Heightmap 接口
#include "IAL/I_Heightmap.h" //

// (前向声明在 B_Mesh.h 中已经完成)
class Mesh; 

namespace NCLGL_Impl {

    /**
     * @brief 轨道 B (nclgl) 高度图实现
     * @details
     * - 实现了 Engine::IAL::I_Heightmap 接口。
     * - 继承自 B_Mesh 以复用 Draw() 功能。
     * - NFR-11.3: B_Factory 将自研 .raw 加载器并手动填充一个 nclgl::Mesh，
     * 然后将其传入此类的构造函数。
     */
    class B_Heightmap : public B_Mesh, public virtual Engine::IAL::I_Heightmap {
    public:
        /**
         * @brief 构造函数
         * @param nclMesh B_Factory 手动填充了地形数据的 nclgl::Mesh 实例
         */
        B_Heightmap(Mesh* nclMesh);
        virtual ~B_Heightmap() = default;

        // (可选) 如果需要实现 I_Heightmap::GetVertex()，
        // 你需要在这里添加 nclgl::Mesh* 的引用，并实现查询逻辑
    };

} // namespace NCLGL_Impl