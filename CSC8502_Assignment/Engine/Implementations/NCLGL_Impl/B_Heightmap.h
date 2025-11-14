/**
* @file B_Heightmap.h
 * @brief 轨道 B (NCLGL_Impl) 的高度图接口实现。
 *
 * 本文件定义了 B_Heightmap 类，它是 Engine::IAL::I_Heightmap 接口在 nclgl 框架下的具体适配器实现。
 * 由于 nclgl 没有专门的 Heightmap 类，它本质上仍然是包装一个 nclgl::Mesh 对象，
 * 但该 Mesh 对象是通过专门的高度图加载逻辑（将在 B_Factory 中实现）创建的。
 *
 * B_Heightmap 类 (NCLGL_Impl::B_Heightmap):
 * 继承自 Engine::IAL::I_Heightmap 纯虚接口。
 * 内部持有一个指向 nclgl::Mesh 的原生指针，并接管其生命周期。
 *
 * 构造函数 B_Heightmap(::Mesh* mesh):
 * 接收一个 nclgl::Mesh 指针。此 Mesh 通常包含地形数据。
 *
 * 析构函数 ~B_Heightmap():
 * 负责释放内部持有的 m_mesh 资源。
 *
 * 成员函数 Draw():
 * 实现 I_Mesh::Draw 接口（通过 I_Heightmap 继承）。
 * 调用底层 m_mesh->Draw()。
 *
 * 成员变量 m_mesh:
 * 指向包含高度图数据的原生 nclgl::Mesh 对象。
 */
#pragma once
#include "IAL/I_Heightmap.h"
#include "nclgl/Vector3.h"
#include <vector>

class Mesh;

namespace NCLGL_Impl {

    class B_Heightmap : public virtual Engine::IAL::I_Heightmap {
    public:
        B_Heightmap(::Mesh* mesh,
                    std::vector<float> samples,
                    size_t dimension,
                    const Vector3& scale);
        ~B_Heightmap() override;

        void Draw() override;

        float SampleHeight(float x, float z) const override;
        Vector3 GetWorldScale() const override;
        Vector2 GetResolution() const override;
        const Engine::IAL::PBRMaterial* GetPBRMaterial() const override;
        void SetPBRMaterial(const Engine::IAL::PBRMaterial& material);

    private:
        ::Mesh* m_mesh;
        std::vector<float> m_samples;
        size_t m_dimension;
        Vector3 m_scale;
        bool m_hasMaterial;
        Engine::IAL::PBRMaterial m_pbrMaterial;
    };

}