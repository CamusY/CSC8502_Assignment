#pragma once

#include "B_Mesh.h"            // 1. (规范) 动画网格也是一种网格
#include "IAL/I_AnimatedMesh.h"// 2. (规范) 实现 I_AnimatedMesh 接口
#include <memory>              // 用于 std::unique_ptr
#include <vector>

// 3. (规范) IAL 接口 
// 和 NFR-13 要求直接使用 nclgl 数学库
#include "nclgl/Matrix4.h"     //

// 4. (规范) 头文件前向声明
class Mesh;
class MeshAnimation;

namespace NCLGL_Impl {

    /**
     * @brief 轨道 B (nclgl) 骨骼动画网格实现
     * @details
     * - 实现了 Engine::IAL::I_AnimatedMesh 接口。
     * - 继承自 B_Mesh 以复用 Draw() 功能。
     * - NFR-11.2: 包装 nclgl::Mesh 
     * 和 nclgl::MeshAnimation。
     * - NFR-13: 实现了 GetBoneTransforms() 这一“泄漏抽象”。
     */
    class B_AnimatedMesh : public B_Mesh, public virtual Engine::IAL::I_AnimatedMesh {
    public:
        /**
         * @brief 构造函数
         * @param nclMesh B_Factory 加载的 .msh 文件实例
         * @param nclAnimation B_Factory 加载的 .anm 文件实例
         */
        B_AnimatedMesh(Mesh* nclMesh, MeshAnimation* nclAnimation);
        virtual ~B_AnimatedMesh();

        // --- I_AnimatedMesh 接口实现 ---

        /**
         * @brief 推进动画时间
         * @param dt 帧时间增量（秒）
         */
        virtual void UpdateAnimation(float dt) override;
        
        /**
         * @brief NFR-13 泄漏抽象：获取当前帧的骨骼变换矩阵
         * @details
         * - Demo 层的 Renderer 将调用此函数获取骨骼矩阵
         * 以上传到着色器。
         * - 内部调用 nclgl::MeshAnimation::GetJointData() 
         * 并将 C 数组转换为 std::vector。
         * @return const std::vector<Matrix4>& 骨骼变换矩阵
         */
        virtual const std::vector<Matrix4>& GetBoneTransforms() const override;

    private:
        std::unique_ptr<MeshAnimation> m_nclAnimation; // 5. (规范) 持有动画数据
        float m_frameTime;                             // 6. (规范) 跟踪当前动画时间
        
        // 7. (规范) NFR-13 关键实现：
        //    用于存储从 GetJointData() 
        //    转换来的 C++ 风格向量，以便安全返回 const&
        std::vector<Matrix4> m_boneTransforms;
    };

} // namespace NCLGL_Impl