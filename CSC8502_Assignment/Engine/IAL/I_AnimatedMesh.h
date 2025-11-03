#pragma once

#include <vector>

// NFR-2: 接口直接依赖 nclgl 通用数学库
#include "nclgl/Matrix4.h"

// IAL 依赖 IAL (纯净)
#include "IAL/I_Mesh.h"

namespace Engine::IAL {

    /**
     * @brief IAL 动画网格接口 (P-4)
     * @details
     * - 继承自 I_Mesh，因为它首先是一个可绘制的网格。
     * - 抽象了 nclgl::Mesh 和 nclgl::MeshAnimation 的组合。
     * - UpdateAnimation: 推进动画计时器。
     * - GetBoneTransforms: NFR-13 中定义的“泄漏抽象”。
     * 它返回一个 Matrix4 向量，用于上传到着色器的 uniform 数组（skinning.vert）。
     * B_AnimatedMesh (轨道 B) 的实现将负责调用 nclgl::MeshAnimation::GetJointData
     * 并将其返回的 C-Style 数组 (const Matrix4*) 转换为 std::vector。
     */
    class I_AnimatedMesh : public I_Mesh {
    public:
        virtual ~I_AnimatedMesh() {}
        
        virtual void UpdateAnimation(float dt) = 0;
        
        virtual const std::vector<Matrix4>& GetBoneTransforms() const = 0;
    };

} // namespace Engine::IAL