/**
* @file B_AnimatedMesh.h
 * @brief 轨道 B (NCLGL_Impl) 的骨骼动画网格接口实现。
 *
 * 本文件定义了 B_AnimatedMesh 类，它是 Engine::IAL::I_AnimatedMesh 接口在 nclgl 框架下的具体适配器实现。
 * 它同时包装了 nclgl::Mesh (用于渲染几何体) 和 nclgl::MeshAnimation (用于骨骼动画数据)。
 *
 * B_AnimatedMesh 类 (NCLGL_Impl::B_AnimatedMesh):
 * 继承自 Engine::IAL::I_AnimatedMesh 纯虚接口。
 * 作为一个适配器，它拥有并管理底层的 nclgl 对象。
 *
 * 构造函数 B_AnimatedMesh(::Mesh* mesh, ::MeshAnimation* anim):
 * 接收 nclgl::Mesh 和 nclgl::MeshAnimation 的原生指针。
 * 参数 mesh: 指向静态网格数据的指针。
 * 参数 anim: 指向动画数据的指针。
 * B_AnimatedMesh 将接管这两个指针的生命周期。
 *
 * 析构函数 ~B_AnimatedMesh():
 * 负责释放 m_mesh 和 m_anim 资源。
 *
 * 成员函数 Draw():
 * 实现 I_Mesh::Draw 接口，调用 m_mesh->Draw()。
 *
 * 成员函数 UpdateAnimation(float dt):
 * 实现 I_AnimatedMesh::UpdateAnimation 接口。
 * 在完整实现中，负责更新动画帧并计算骨骼变换矩阵。
 *
 * 成员函数 GetBoneTransforms():
 * 实现 I_AnimatedMesh::GetBoneTransforms 接口。
 * 返回当前帧所有骨骼的变换矩阵数组，用于传递给着色器。
 *
 * 成员变量 m_mesh:
 * 指向原生的 nclgl::Mesh 对象。
 *
 * 成员变量 m_anim:
 * 指向原生的 nclgl::MeshAnimation 对象。
 *
 * 成员变量 m_boneTransforms:
 * 缓存当前帧计算出的骨骼变换矩阵，供 GetBoneTransforms 返回引用使用。
 */
#pragma once
#include "IAL/I_AnimatedMesh.h"
#include <vector>

class Mesh;
class MeshAnimation;

namespace NCLGL_Impl {

    class B_AnimatedMesh : public virtual Engine::IAL::I_AnimatedMesh {
    public:
        B_AnimatedMesh(::Mesh* mesh, ::MeshAnimation* anim);
        ~B_AnimatedMesh() override;

        void Draw() override;
        void UpdateAnimation(float dt) override;
        const std::vector<Matrix4>& GetBoneTransforms() const override;

    private:
        ::Mesh* m_mesh;
        ::MeshAnimation* m_anim;
        std::vector<Matrix4> m_boneTransforms;
    };

}