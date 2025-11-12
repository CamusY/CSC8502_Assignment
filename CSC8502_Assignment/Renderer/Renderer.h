/**
* @file Renderer.h
 * @brief 声明负责遍历场景图并提交绘制命令的 Renderer 类。
 * @details
 * Renderer 持有资源工厂引用、场景图指针与相机实例，通过 CollectRenderableNodes 收集节点，
 * 在 Render 函数中遍历并调用 I_Mesh::Draw()。Day11 阶段增加了天空盒与光照支持：
 * 在渲染场景几何之前绘制 cubemap 天空盒，并在地形着色器中注入 Blinn-Phong 光照所需的光源与相机参数。
 */
#pragma once

#include <memory>
#include <vector>

#include "../Core/SceneGraph.h"
#include "../Core/Light.h"
#include "../Engine/IAL/I_ResourceFactory.h"

#include "nclgl/Vector3.h"

namespace Engine::IAL {
    class I_Shader;
    class I_Texture;
    class I_Mesh;
}

class PostProcessing;
class Camera;

class Renderer {
public:
    Renderer(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
             const std::shared_ptr<SceneGraph>& sceneGraph,
             const std::shared_ptr<Camera>& camera,
             int width,
             int height);

    void Render();

private:
    std::shared_ptr<Engine::IAL::I_ResourceFactory> m_factory;
    std::shared_ptr<SceneGraph> m_sceneGraph;
    std::shared_ptr<Camera> m_camera;
    std::vector<std::shared_ptr<SceneNode>> m_renderQueue;
    std::shared_ptr<PostProcessing> m_postProcessing;
    std::shared_ptr<Engine::IAL::I_Shader> m_sceneShader;
    std::shared_ptr<Engine::IAL::I_Shader> m_postShader;
    std::shared_ptr<Engine::IAL::I_Shader> m_terrainShader;
    std::shared_ptr<Engine::IAL::I_Shader> m_skyboxShader;
    std::shared_ptr<Engine::IAL::I_Texture> m_skyboxTexture;
    std::shared_ptr<Engine::IAL::I_Mesh> m_skyboxMesh;
    Light m_directionalLight;
    Vector3 m_sceneColour;
    float m_specularPower;
    float m_nearPlane;
    float m_farPlane;
    int m_surfaceWidth;
    int m_surfaceHeight;
};