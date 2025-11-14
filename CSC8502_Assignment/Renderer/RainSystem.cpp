/**
 * @file RainSystem.cpp
 * @brief 实现围绕相机的雨滴粒子系统。
 */
#include "RainSystem.h"

#include <algorithm>
#include "IAL/I_ResourceFactory.h"
#include "IAL/I_Shader.h"
#include "IAL/I_Heightmap.h"

RainSystem::RainSystem(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
                       int maxParticles,
                       float horizontalExtent,
                       float verticalExtent)
    : m_shader(nullptr)
    , m_particles()
    , m_gpuBuffer()
    , m_random(std::random_device{}())
    , m_vao(0)
    , m_vertexVbo(0)
    , m_instanceVbo(0)
    , m_maxParticles(std::max(maxParticles, 1))
    , m_horizontalExtent(std::max(horizontalExtent, 1.0f))
    , m_verticalExtent(std::max(verticalExtent, 1.0f))
    , m_minSpeed(35.0f)
    , m_maxSpeed(80.0f)
    , m_minLength(6.0f)
    , m_maxLength(22.0f)
    , m_width(0.35f)
    , m_resourcesReady(false)
    , m_needUpload(false) {
    m_particles.resize(static_cast<std::size_t>(m_maxParticles));
    m_gpuBuffer.resize(static_cast<std::size_t>(m_maxParticles));
    InitializeResources(factory);
}

RainSystem::~RainSystem() {
    if (m_instanceVbo != 0) {
        glDeleteBuffers(1, &m_instanceVbo);
        m_instanceVbo = 0;
    }
    if (m_vertexVbo != 0) {
        glDeleteBuffers(1, &m_vertexVbo);
        m_vertexVbo = 0;
    }
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}

void RainSystem::InitializeResources(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory) {
    if (!factory) {
        return;
    }

    struct Corner {
        float x;
        float y;
    };
    const Corner quad[4] = {
        {-0.5f, 0.0f},
        {0.5f, 0.0f},
        {-0.5f, 1.0f},
        {0.5f, 1.0f}
    };

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vertexVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Corner), reinterpret_cast<void*>(0));

    glGenBuffers(1, &m_instanceVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(m_particles.size() * sizeof(Particle)),
                 nullptr,
                 GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Particle),
                          reinterpret_cast<void*>(0));
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
                          1,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Particle),
                          reinterpret_cast<void*>(offsetof(Particle, speed)));
    glVertexAttribDivisor(2, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_shader = factory->CreateShader("Shared/rain.vert", "Shared/rain.frag");
    if (m_shader) {
        m_resourcesReady = true;
    }
}

void RainSystem::InitializeParticles(const Vector3& cameraPosition) {
    std::uniform_real_distribution<float> offsetDist(-m_horizontalExtent, m_horizontalExtent);
    std::uniform_real_distribution<float> heightDist(0.0f, m_verticalExtent);
    std::uniform_real_distribution<float> speedDist(m_minSpeed, m_maxSpeed);

    for (auto& particle : m_particles) {
        particle.position = Vector3(cameraPosition.x + offsetDist(m_random),
                                    cameraPosition.y + heightDist(m_random),
                                    cameraPosition.z + offsetDist(m_random));
        particle.speed = speedDist(m_random);
    }
    m_needUpload = true;
}

void RainSystem::Update(float deltaTime,
                        const Vector3& cameraPosition,
                        float cameraYaw,
                        float cameraPitch,
                        const std::shared_ptr<Engine::IAL::I_Heightmap>& heightmap,
                        float waterHeight) {
    if (!m_resourcesReady) {
        return;
    }

    (void)cameraYaw;
    (void)cameraPitch;

    if (!m_needUpload) {
        InitializeParticles(cameraPosition);
    }

    std::uniform_real_distribution<float> offsetDist(-m_horizontalExtent, m_horizontalExtent);
    std::uniform_real_distribution<float> speedDist(m_minSpeed, m_maxSpeed);

    for (std::size_t i = 0; i < m_particles.size(); ++i) {
        auto& particle = m_particles[i];
        particle.position.y -= particle.speed * deltaTime;

        const float boxSize = m_horizontalExtent * 2.0f;
        float dx = particle.position.x - cameraPosition.x;
        while (dx > m_horizontalExtent) {
            particle.position.x -= boxSize;
            dx -= boxSize;
        }
        while (dx < -m_horizontalExtent) {
            particle.position.x += boxSize;
            dx += boxSize;
        }
        float dz = particle.position.z - cameraPosition.z;
        while (dz > m_horizontalExtent) {
            particle.position.z -= boxSize;
            dz -= boxSize;
        }
        while (dz < -m_horizontalExtent) {
            particle.position.z += boxSize;
            dz += boxSize;
        }

        float terrainHeight = 0.0f;
        if (heightmap) {
            terrainHeight = heightmap->SampleHeight(particle.position.x, particle.position.z);
        }
        const float lowerBound = std::max(terrainHeight, waterHeight);
        if (particle.position.y <= lowerBound) {
            particle.position.x = cameraPosition.x + offsetDist(m_random);
            particle.position.z = cameraPosition.z + offsetDist(m_random);
            particle.position.y = cameraPosition.y + m_verticalExtent;
            particle.speed = speedDist(m_random);
        }

        m_gpuBuffer[i] = particle;
    }
    m_needUpload = true;
}

void RainSystem::UploadInstanceData() {
    if (!m_needUpload || m_instanceVbo == 0) {
        return;
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVbo);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    static_cast<GLsizeiptr>(m_gpuBuffer.size() * sizeof(Particle)),
                    m_gpuBuffer.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_needUpload = false;
}

Vector3 RainSystem::ComputeForward(float yawDegrees, float pitchDegrees) {
    Matrix4 yawMatrix = Matrix4::Rotation(yawDegrees, Vector3(0.0f, 1.0f, 0.0f));
    Matrix4 pitchMatrix = Matrix4::Rotation(pitchDegrees, Vector3(1.0f, 0.0f, 0.0f));
    Matrix4 orientation = yawMatrix * pitchMatrix;
    Vector3 forward = orientation * Vector3(0.0f, 0.0f, -1.0f);
    forward.Normalise();
    return forward;
}

void RainSystem::Render(const Matrix4& view,
                        const Matrix4& projection,
                        const Vector3& cameraPosition,
                        float cameraYaw,
                        float cameraPitch,
                        const Vector3& fogColor,
                        float fogDensity) {
    if (!m_resourcesReady || !m_shader || m_vao == 0) {
        return;
    }

    UploadInstanceData();

    Vector3 forward = ComputeForward(cameraYaw, cameraPitch);
    Vector3 right = Vector3::Cross(forward, Vector3(0.0f, 1.0f, 0.0f));
    if (right.Length() < 1e-4f) {
        right = Vector3(1.0f, 0.0f, 0.0f);
    }
    right.Normalise();
    Vector3 rainDirection(0.0f, -1.0f, 0.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    m_shader->Bind();
    m_shader->SetUniform("uView", view);
    m_shader->SetUniform("uProj", projection);
    m_shader->SetUniform("uCameraPos", cameraPosition);
    m_shader->SetUniform("uCameraRight", right);
    m_shader->SetUniform("uRainDirection", rainDirection);
    m_shader->SetUniform("uMinSpeed", m_minSpeed);
    m_shader->SetUniform("uMaxSpeed", m_maxSpeed);
    m_shader->SetUniform("uMinLength", m_minLength);
    m_shader->SetUniform("uMaxLength", m_maxLength);
    m_shader->SetUniform("uWidth", m_width);
    m_shader->SetUniform("uFogColor", fogColor);
    m_shader->SetUniform("uFogDensity", fogDensity);
    m_shader->SetUniform("uBaseColor", Vector3(0.66f, 0.76f, 0.92f));

    glBindVertexArray(m_vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_maxParticles);
    glBindVertexArray(0);

    m_shader->Unbind();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}