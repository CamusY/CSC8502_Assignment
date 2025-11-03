#include "B_AnimatedMesh.h"

// 8. (规范) 在 .cpp 中包含具体实现
#include "nclgl/Mesh.h"
#include "nclgl/MeshAnimation.h" //
#include <stdexcept>
#include <algorithm> // 用于 std::copy

namespace NCLGL_Impl {

    B_AnimatedMesh::B_AnimatedMesh(Mesh* nclMesh, MeshAnimation* nclAnimation)
        : B_Mesh(nclMesh), // 9. (规范) 将 Mesh 传递给父类
          m_nclAnimation(nclAnimation),
          m_frameTime(0.0f) {
        
        if (!m_nclAnimation) {
            throw std::runtime_error("B_AnimatedMesh 构造失败：传入的 nclAnimation 为空指针。");
        }
        if (!nclMesh) {
             // B_Mesh 构造函数已经检查过了，但双重保险无害
            throw std::runtime_error("B_AnimatedMesh 构造失败：传入的 nclMesh 为空指针。");
        }

        // 10. (规范) NFR-13 
        //      为骨骼变换向量分配内存，大小为 nclgl 动画的骨骼数
        m_boneTransforms.resize(m_nclAnimation->GetJointCount()); //
    }

    B_AnimatedMesh::~B_AnimatedMesh() {
        // m_nclAnimation 会被 std::unique_ptr 自动 delete
        // m_nclMesh 会被父类 B_Mesh 的析构函数自动 delete
    }

    void B_AnimatedMesh::UpdateAnimation(float dt) {
        // 11. (规范) 推进动画时间
        m_frameTime += dt;
        
        const float frameRate = m_nclAnimation->GetFrameRate(); //
        const int frameCount = m_nclAnimation->GetFrameCount(); //
        if (frameCount == 0) return; // 安全检查

        // 12. (规范) 动画循环逻辑
        // nclgl 动画帧的时长
        const float frameLength = 1.0f / frameRate; 
        const float animLength = frameCount * frameLength;

        while(m_frameTime > animLength) {
            m_frameTime -= animLength;
        }
        
        // 13. (规范) 计算当前帧索引
        int currentFrame = static_cast<int>(m_frameTime / frameLength);
        currentFrame = std::min(currentFrame, frameCount - 1); // 钳制到最后一帧

        // 14. (规范) NFR-13 关键实现
        //      从 nclgl 获取 C 风格的矩阵数组
        const Matrix4* nclBoneData = m_nclAnimation->GetJointData(currentFrame); //
        
        if (nclBoneData) {
            // 15. (规范) NFR-13
            //      将 C 数组数据复制到我们的 std::vector 成员变量中
            std::copy(nclBoneData, 
                      nclBoneData + m_boneTransforms.size(), 
                      m_boneTransforms.begin());
        }
    }

    const std::vector<Matrix4>& B_AnimatedMesh::GetBoneTransforms() const {
        // 16. (规范) NFR-13
        //      返回我们内部 std::vector 的 const 引用
        return m_boneTransforms;
    }

} // namespace NCLGL_Impl