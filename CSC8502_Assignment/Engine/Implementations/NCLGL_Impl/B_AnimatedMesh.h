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
 * 构造函数 B_AnimatedMesh(std::shared_ptr<::Mesh> mesh, std::shared_ptr<::MeshAnimation> anim):
 * 接收共享指针形式的 nclgl::Mesh 与 nclgl::MeshAnimation，并保存所有权。
 * 参数 mesh: 指向静态网格数据的共享指针。
 * 参数 anim: 指向动画数据的共享指针。
 * 另提供 B_AnimatedMesh(::Mesh*, ::MeshAnimation*) 重载以便处理旧接口，内部会转换为共享指针管理。
 *
 * 析构函数 ~B_AnimatedMesh():
 * 依赖智能指针自动回收底层资源，无需手动 delete。
 *
 * 成员函数 Draw():
 * 实现 I_Mesh::Draw 接口，调用 m_mesh->Draw()。
 *
 * 成员函数 UpdateAnimation(float dt):
 * 推进内部时间累积并根据动画帧率循环帧索引，随后调用 CacheBoneTransforms。
 *
 * 成员函数 GetBoneTransforms():
 * 实现 I_AnimatedMesh::GetBoneTransforms 接口。
 * 返回当前帧所有骨骼的变换矩阵数组，用于传递给着色器。
 *
 * 成员变量 m_mesh:
 * 指向底层 nclgl::Mesh 对象的共享指针。
 *
 * 成员变量 m_anim:
 * 指向底层 nclgl::MeshAnimation 对象的共享指针。
 *
 * 成员变量 m_boneTransforms:
 * 缓存当前帧的骨骼变换矩阵，供 GetBoneTransforms 返回引用使用。
 *
 * 成员变量 m_timeAccumulator:
 * 累积时间以便依据帧率驱动动画播放。
 *
 * 成员变量 m_currentFrame:
 * 当前播放的帧索引。
 */
#pragma once
#include "IAL/I_AnimatedMesh.h"
#include "IAL/I_Texture.h"
#include <memory>
#include <vector>

class Mesh;
class MeshAnimation;

namespace NCLGL_Impl {

    class B_AnimatedMesh : public virtual Engine::IAL::I_AnimatedMesh {
    public:
        B_AnimatedMesh(std::shared_ptr<::Mesh> mesh, std::shared_ptr<::MeshAnimation> anim);
        B_AnimatedMesh(::Mesh* mesh, ::MeshAnimation* anim);
        ~B_AnimatedMesh() override;

        void Draw() override;
        void UpdateAnimation(float dt) override;
        const std::vector<Matrix4>& GetBoneTransforms() const override;
        Matrix4 GetRootTransform() const override;
        std::shared_ptr<Engine::IAL::I_Texture> GetDefaultTexture() const override;

        const Engine::IAL::PBRMaterial* GetPBRMaterial() const override;

        void SetRootTransform(const Matrix4& transform);
        void SetDefaultTexture(const std::shared_ptr<Engine::IAL::I_Texture>& texture);
        void SetPBRMaterial(const Engine::IAL::PBRMaterial& material);

    private:
        void CacheBoneTransforms();

        std::shared_ptr<::Mesh> m_mesh;
        std::shared_ptr<::MeshAnimation> m_anim;
        std::vector<Matrix4> m_boneTransforms;
        float m_timeAccumulator;
        unsigned int m_currentFrame;
        Matrix4 m_rootTransform;
        std::shared_ptr<Engine::IAL::I_Texture> m_defaultTexture;
        bool m_hasPBR = false;
        Engine::IAL::PBRMaterial m_pbrMaterial;
    };

}
