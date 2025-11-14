/**
* @file B_Heightmap.cpp
 * @brief 轨道 B (NCLGL_Impl) 的高度图接口实现源文件。
 *
 * 本文件实现了 B_Heightmap 类。
 * 在 Day 2 阶段，它是一个空壳实现，析构函数包含基本的资源释放逻辑。
 */
#include "B_Heightmap.h"
#include "nclgl/Mesh.h"
#include "nclgl/Vector4.h"
#include <algorithm>
#include <cmath>

namespace NCLGL_Impl {

    B_Heightmap::B_Heightmap(::Mesh* mesh,
                             std::vector<float> samples,
                             size_t dimension,
                             const Vector3& scale)
        : m_mesh(mesh)
        , m_samples(std::move(samples))
        , m_dimension(dimension)
        , m_scale(scale)
        , m_hasMaterial(true) {
        m_pbrMaterial.baseColorFactor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        m_pbrMaterial.metallicFactor = 0.0f;
        m_pbrMaterial.roughnessFactor = 1.0f;
        m_pbrMaterial.alphaMode = Engine::IAL::AlphaMode::Opaque;
        m_pbrMaterial.doubleSided = false;
    }

    B_Heightmap::~B_Heightmap() {
        delete m_mesh;
    }

    void B_Heightmap::Draw() {
        if (m_mesh) {
            m_mesh->Draw();
        }
    }

    float B_Heightmap::SampleHeight(float x, float z) const {
        if (m_dimension == 0 || m_samples.empty()) {
            return 0.0f;
        }
        const float scaledX = std::clamp(x / m_scale.x, 0.0f, static_cast<float>(m_dimension - 1));
        const float scaledZ = std::clamp(z / m_scale.z, 0.0f, static_cast<float>(m_dimension - 1));

        const std::size_t x0 = static_cast<std::size_t>(std::floor(scaledX));
        const std::size_t x1 = std::min<std::size_t>(x0 + 1, m_dimension - 1);
        const std::size_t z0 = static_cast<std::size_t>(std::floor(scaledZ));
        const std::size_t z1 = std::min<std::size_t>(z0 + 1, m_dimension - 1);

        const float tx = scaledX - static_cast<float>(x0);
        const float tz = scaledZ - static_cast<float>(z0);

        auto sample = [&](std::size_t sx, std::size_t sz) {
            const std::size_t index = sz * m_dimension + sx;
            return m_samples[index];
        };

        const float h00 = sample(x0, z0);
        const float h10 = sample(x1, z0);
        const float h01 = sample(x0, z1);
        const float h11 = sample(x1, z1);

        const float hx0 = std::lerp(h00, h10, tx);
        const float hx1 = std::lerp(h01, h11, tx);
        const float height = std::lerp(hx0, hx1, tz);

        return height * m_scale.y;
    }

    Vector3 B_Heightmap::GetWorldScale() const {
        return m_scale;
    }

    Vector2 B_Heightmap::GetResolution() const {
        return Vector2(static_cast<float>(m_dimension), static_cast<float>(m_dimension));
    }

    const Engine::IAL::PBRMaterial* B_Heightmap::GetPBRMaterial() const {
        return m_hasMaterial ? &m_pbrMaterial : nullptr;
    }

    void B_Heightmap::SetPBRMaterial(const Engine::IAL::PBRMaterial& material) {
        m_pbrMaterial = material;
        m_hasMaterial = true;
    }

}