/**
* @file I_AnimatedMesh.h
 * @brief 定义了可播放骨骼动画的网格（Animated Mesh）的抽象接口。
 * @details
 * 该文件的设计目的是为核心渲染器（Renderer）提供一个统一的、可更新动画和提取骨骼
 * 变换的接口，以满足高级功能 P-4（骨骼动画）的需求。
 * 此接口继承自 I_Mesh，因此也可以被场景图（SceneGraph） 和渲染器像普通网格一样
 * 绘制（调用 Draw()）。
 *
 * (NFR-1) 规范约束：本文件严禁包含 nclgl/Mesh.h 或 nclgl/MeshAnimation.h。
 * (NFR-2) 规范要求：本文件被授权且必须包含 nclgl 数学库（如 Matrix4.h），
 * 以便在函数签名中直接使用 nclgl 的具体数学类型。
 *
 * @see I_ResourceFactory::LoadAnimatedMesh
 * @see I_Mesh
 *
 * @class Engine::IAL::I_AnimatedMesh
 * @brief 骨骼动画网格的纯虚接口。
 * @details
 * 抽象了 nclgl 库中 nclgl::Mesh 和 nclgl::MeshAnimation 
 * 两个类的组合功能。
 * 实例由 I_ResourceFactory::LoadAnimatedMesh() 创建。
 *
 * @fn Engine::IAL::I_AnimatedMesh::~I_AnimatedMesh
 * @brief 虚拟析构函数。
 *
 * @fn Engine::IAL::I_AnimatedMesh::UpdateAnimation(float dt)
 * @brief 更新此网格的内部动画状态。
 * @details
 * 渲染器或场景管理器应在每帧调用此函数，传入增量时间（dt），
 * 以便动画（例如 nclgl::MeshAnimation）可以推进到下一帧。
 * @param dt 自上一帧以来的增量时间（秒）。
 *
 * @fn Engine::IAL::I_AnimatedMesh::GetBoneTransforms
 * @brief (NFR-13) 获取当前动画帧的所有骨骼（关节）的世界变换矩阵。
 * @details
 * (NFR-13 架构妥协) 这是一个有意的“泄漏抽象”（Leaky Abstraction）。
 * * 1.  **设计目的**：此接口直接服务于 GPU 蒙皮着色器（skinning.vert） 
 * 对 `uniform mat4 boneMatrices[]` 数组的需求。
 * 2.  **轨道 B 映射**：在轨道 B（nclgl）实现中，`B_AnimatedMesh` 将调用
 * `nclgl::MeshAnimation::GetJointData()`，该函数返回一个
 * `const Matrix4*` 指针。`B_AnimatedMesh` 适配器负责将这个C风格数组
 * 转换为 `std::vector<Matrix4>`。
 * 3.  **约束**：我们接受此接口设计缺乏演化性（例如不支持动画混合），
 * 以换取在轨道 B 上的可实现性并满足 P-4 验收标准（AC-P4.2）。
 *
 * @return 一个包含所有骨骼变换矩阵的 `std::vector` 的常量引用。
 * 向量的索引对应于着色器中骨骼 ID。
 */

#pragma once

#include <vector>

#include "nclgl/Matrix4.h"

#include "IAL/I_Mesh.h"

namespace Engine::IAL {
    class I_AnimatedMesh : public virtual I_Mesh {
    public:
        virtual ~I_AnimatedMesh() {}

        virtual void UpdateAnimation(float dt) = 0;

        virtual const std::vector<Matrix4>& GetBoneTransforms() const = 0;
        
        virtual Matrix4 GetRootTransform() const {
            Matrix4 identity;
            identity.ToIdentity();
            return identity;
        }
    };

}
