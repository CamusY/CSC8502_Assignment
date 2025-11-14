/**
 * @file GrassField.h
 * @brief 声明基于高度图实例化草地的辅助渲染类。
 * @details
 * GrassField 负责根据传入的 Heightmap 采样一批种子点，并通过几何着色器
 * 将每个点扩展成草叶。Renderer 在水体与主场景之间调用 Render，以与
 * 反射/折射结果共享相同的视图投影矩阵。类内部缓存 VAO/VBO 与草地着色器，
 * 允许在不同帧重复渲染而无需重新生成实例数据。
 */
#pragma once
#include <memory>
#include "nclgl/Matrix4.h"
#include "nclgl/Vector3.h"

namespace Engine::IAL {
    class I_ResourceFactory;
    class I_Heightmap;
    class I_Shader;
    class I_Texture;
}

class GrassField {
public:
    GrassField(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
               const std::shared_ptr<Engine::IAL::I_Heightmap>& heightmap,
               float waterHeight);
    ~GrassField();

    void Render(const Matrix4& view,
                    const Matrix4& projection,
                    const Vector3& cameraPosition,
                    float timeSeconds,
                    int debugMode,
                    float nearPlane,
                    float farPlane);

    void SetColor(const Vector3& color);
    void SetBaseColorTexture(const std::shared_ptr<Engine::IAL::I_Texture>& texture);

private:
    void GenerateInstances(const std::shared_ptr<Engine::IAL::I_Heightmap>& heightmap,
                           float waterHeight);

    std::shared_ptr<Engine::IAL::I_Shader> m_shader;
    unsigned int m_vao;
    unsigned int m_vbo;
    int m_instanceCount;
    Vector3 m_color;
    std::shared_ptr<Engine::IAL::I_Texture> m_defaultBaseColorTexture;
    std::shared_ptr<Engine::IAL::I_Texture> m_baseColorTexture;
    std::shared_ptr<Engine::IAL::I_Texture> m_alphaShapeTexture;
    float m_fallbackAlpha;
};