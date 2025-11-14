/**
 * @file RainSystem.h
 * @brief 声明粒子化雨滴渲染系统。
 *
 * @details
 * RainSystem 负责维护围绕相机的“无限雨盒子”，更新雨滴的生命周期并以 GPU 实例化方式绘制雨丝。
 * Update 会根据相机位置包裹粒子坐标、依据高度图与水面高度决定雨滴重生高度，Render 则上传实例数据并输出半透明雨线。
 */
#pragma once

#include <memory>
#include <random>
#include <vector>

#include <glad/glad.h>

#include "nclgl/Matrix4.h"
#include "nclgl/Vector3.h"

namespace Engine::IAL {
    class I_Shader;
    class I_ResourceFactory;
    class I_Heightmap;
}

class RainSystem {
public:
    RainSystem(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
               int maxParticles,
               float horizontalExtent,
               float verticalExtent);
    ~RainSystem();

    void Update(float deltaTime,
                const Vector3& cameraPosition,
                float cameraYaw,
                float cameraPitch,
                const std::shared_ptr<Engine::IAL::I_Heightmap>& heightmap,
                float waterHeight);

    void Render(const Matrix4& view,
                const Matrix4& projection,
                const Vector3& cameraPosition,
                float cameraYaw,
                float cameraPitch,
                const Vector3& fogColor,
                float fogDensity,
                float farPlane);

private:
    struct Particle {
        Vector3 position;
        float speed;
    };

    void InitializeResources(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory);
    void InitializeParticles(const Vector3& cameraPosition);
    void UploadInstanceData();
    static Vector3 ComputeForward(float yawDegrees, float pitchDegrees);

    std::shared_ptr<Engine::IAL::I_Shader> m_shader;
    std::vector<Particle> m_particles;
    std::vector<Particle> m_gpuBuffer;
    std::mt19937 m_random;
    GLuint m_vao;
    GLuint m_vertexVbo;
    GLuint m_instanceVbo;
    int m_maxParticles;
    float m_horizontalExtent;
    float m_verticalExtent;
    float m_minSpeed;
    float m_maxSpeed;
    float m_minLength;
    float m_maxLength;
    float m_width;
    bool m_resourcesReady;
    bool m_needUpload;
};