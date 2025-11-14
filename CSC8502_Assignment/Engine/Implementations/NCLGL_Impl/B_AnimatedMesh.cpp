/**
* @file B_AnimatedMesh.cpp
 * @brief 轨道 B (NCLGL_Impl) 的骨骼动画网格接口实现源文件。
 *
 * 本文件实现了 B_AnimatedMesh 类。
 * 在 Day 2 阶段，所有函数均为基础的空壳实现 (Stubs)。
 * 析构函数已包含基础的资源释放逻辑。
 */
#include "B_AnimatedMesh.h"
#include "nclgl/Mesh.h"
#include "nclgl/MeshAnimation.h"
#include <algorithm>

namespace NCLGL_Impl {

    namespace {
        constexpr float kMinFrameRate = 1e-4f;
    }

    B_AnimatedMesh::B_AnimatedMesh(std::shared_ptr<::Mesh> mesh, std::shared_ptr<::MeshAnimation> anim) :
        m_mesh(std::move(mesh))
        , m_anim(std::move(anim))
        , m_boneTransforms()
        , m_timeAccumulator(0.0f)
        , m_currentFrame(0)
        , m_rootTransform()
        , m_defaultTexture(nullptr) {
        m_rootTransform.ToIdentity();
        CacheBoneTransforms();
    }

    B_AnimatedMesh::B_AnimatedMesh(::Mesh* mesh, ::MeshAnimation* anim) :
        B_AnimatedMesh(std::shared_ptr<::Mesh>(mesh), std::shared_ptr<::MeshAnimation>(anim)) {
    }

    B_AnimatedMesh::~B_AnimatedMesh() = default;

    void B_AnimatedMesh::Draw() {
        if (m_mesh) {
            m_mesh->Draw();
        }
    }

    void B_AnimatedMesh::UpdateAnimation(float dt) {
        if (!m_anim) {
            m_boneTransforms.clear();
            return;
        }

        dt = std::max(dt, 0.0f);

        const unsigned int frameCount = m_anim->GetFrameCount();
        if (frameCount == 0) {
            m_boneTransforms.clear();
            return;
        }

        m_timeAccumulator += dt;
        const float frameRate = m_anim->GetFrameRate();
        const float frameDuration = frameRate > kMinFrameRate ? 1.0f / frameRate : 0.0f;

        if (frameDuration > 0.0f) {
            while (m_timeAccumulator >= frameDuration) {
                m_timeAccumulator -= frameDuration;
                m_currentFrame = (m_currentFrame + 1) % frameCount;
            }
        }
        else {
            m_currentFrame = std::min(m_currentFrame, frameCount - 1);
        }

        CacheBoneTransforms();
    }

    const std::vector<Matrix4>& B_AnimatedMesh::GetBoneTransforms() const {
        return m_boneTransforms;
    }

    Matrix4 B_AnimatedMesh::GetRootTransform() const {
        return m_rootTransform;
    }

    std::shared_ptr<Engine::IAL::I_Texture> B_AnimatedMesh::GetDefaultTexture() const {
        return m_defaultTexture;
    }

    const Engine::IAL::PBRMaterial* B_AnimatedMesh::GetPBRMaterial() const {
        return m_hasPBR ? &m_pbrMaterial : nullptr;
    }

    void B_AnimatedMesh::SetRootTransform(const Matrix4& transform) {
        m_rootTransform = transform;
    }

    void B_AnimatedMesh::SetDefaultTexture(const std::shared_ptr<Engine::IAL::I_Texture>& texture) {
        m_defaultTexture = texture;
    }

    void B_AnimatedMesh::SetPBRMaterial(const Engine::IAL::PBRMaterial& material) {
        m_pbrMaterial = material;
        m_hasPBR = true;
    }


    void B_AnimatedMesh::CacheBoneTransforms() {
        if (!m_anim) {
            m_boneTransforms.clear();
            return;
        }

        const unsigned int jointCount = m_anim->GetJointCount();
        if (jointCount == 0) {
            m_boneTransforms.clear();
            return;
        }

        m_boneTransforms.resize(jointCount);

        const Matrix4* jointData = m_anim->GetJointData(m_currentFrame);
        if (!jointData) {
            for (unsigned int i = 0; i < jointCount; ++i) {
                m_boneTransforms[i] = Matrix4();
            }
            return;
        }

        const Matrix4* inverseBindPose = nullptr;

        unsigned int inverseBindPoseCount = 0;

        if (m_mesh) {
            inverseBindPose = m_mesh->GetInverseBindPose();
            inverseBindPoseCount = m_mesh->GetJointCount();

            if (!inverseBindPose || inverseBindPoseCount != jointCount) {
                inverseBindPose = nullptr;
            }
        }

        for (unsigned int i = 0; i < jointCount; ++i) {
            if (inverseBindPose) {
                m_boneTransforms[i] = jointData[i] * inverseBindPose[i];
            }
            else {
                m_boneTransforms[i] = jointData[i];
            }
        }
    }

}
