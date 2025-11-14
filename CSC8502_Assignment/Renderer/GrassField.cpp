/**
 * @file GrassField.cpp
 * @brief 实现基于高度图生成草地实例并绘制的 GrassField 类。
 */

#include "GrassField.h"

#include "../Engine/IAL/I_ResourceFactory.h"
#include "../Engine/IAL/I_Heightmap.h"
#include "../Engine/IAL/I_Shader.h"

#include <glad/glad.h>
#include <random>
#include <vector>
#include "nclgl/Vector4.h"

namespace {
    constexpr int kGrassInstanceCount = 8000;
}

GrassField::GrassField(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
                       const std::shared_ptr<Engine::IAL::I_Heightmap>& heightmap,
                       float waterHeight)
    : m_shader(nullptr)
    , m_vao(0)
    , m_vbo(0)
    , m_instanceCount(0)
    , m_color(Vector3(0.35f, 0.65f, 0.35f)) {
    if (factory) {
        m_shader = factory->CreateShader("Shared/grass.vert", "Shared/grass.frag", "Shared/grass.geom");
    }
    GenerateInstances(heightmap, waterHeight);
}

GrassField::~GrassField() {
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}

void GrassField::SetColor(const Vector3& color) {
    m_color = color;
}

void GrassField::GenerateInstances(const std::shared_ptr<Engine::IAL::I_Heightmap>& heightmap,
                                   float waterHeight) {
    if (!heightmap) {
        return;
    }

    Vector2 resolution = heightmap->GetResolution();
    Vector3 scale = heightmap->GetWorldScale();
    const float maxX = (resolution.x > 1.0f) ? (resolution.x - 1.0f) * scale.x : 0.0f;
    const float maxZ = (resolution.y > 1.0f) ? (resolution.y - 1.0f) * scale.z : 0.0f;

    std::mt19937 rng(1337u);
    std::uniform_real_distribution<float> distX(0.0f, maxX);
    std::uniform_real_distribution<float> distZ(0.0f, maxZ);
    std::uniform_real_distribution<float> distRandom(0.0f, 1.0f);

    std::vector<Vector4> instances;
    instances.reserve(kGrassInstanceCount);

    const int maxAttempts = kGrassInstanceCount * 4;
    for (int attempt = 0; attempt < maxAttempts && static_cast<int>(instances.size()) < kGrassInstanceCount; ++attempt) {
        const float x = distX(rng);
        const float z = distZ(rng);
        const float height = heightmap->SampleHeight(x, z);
        if (height < waterHeight + 0.5f) {
            continue;
        }
        Vector4 instance(x, height, z, distRandom(rng));
        instances.push_back(instance);
    }

    if (instances.empty()) {
        return;
    }

    if (m_vbo == 0) {
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
    }
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(Vector4), instances.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4), reinterpret_cast<void*>(0));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_instanceCount = static_cast<int>(instances.size());
}

void GrassField::Render(const Matrix4& view,
                        const Matrix4& projection,
                        const Vector3& cameraPosition,
                        float timeSeconds) {
    if (!m_shader || m_instanceCount <= 0 || m_vao == 0) {
        return;
    }

    Matrix4 viewProj = projection * view;
    m_shader->Bind();
    m_shader->SetUniform("uViewProj", viewProj);
    m_shader->SetUniform("uCameraPos", cameraPosition);
    m_shader->SetUniform("uTime", timeSeconds);
    m_shader->SetUniform("uColor", m_color);

    GLboolean prevCull = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    GLboolean prevBlend = glIsEnabled(GL_BLEND);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_POINTS, 0, m_instanceCount);
    glBindVertexArray(0);

    if (prevCull) {
        glEnable(GL_CULL_FACE);
    }
    else {
        glDisable(GL_CULL_FACE);
    }
    if (!prevBlend) {
        glDisable(GL_BLEND);
    }

    m_shader->Unbind();
}