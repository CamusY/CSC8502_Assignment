/**
* @file Renderer.h
 * @brief 声明负责遍历场景图并提交绘制命令的 Renderer 类。
 * @details
 * Renderer 持有资源工厂引用、场景图指针与相机实例，通过 CollectRenderableNodes 收集节点，
 * 在 Render 函数中遍历并调用 I_Mesh::Draw()。Day11 阶段增加了天空盒与光照支持：
 * 在渲染场景几何之前绘制 cubemap 天空盒，并在地形着色器中注入 Blinn-Phong 光照所需的光源与相机参数。
 * Day12 进一步扩展了渲染流程，引入水体节点的递归渲染：在主场景绘制前先渲染反射与折射帧缓冲，
 * 随后使用专用水面着色器将两个纹理组合成最终的水体效果。Day13 则结合 I_DebugUI 提供的控制面板，
 * 允许在运行时调整方向光的颜色、位置与镜面强度，从而满足 UI 可调参需求。
 */
#pragma once

#include <memory>
#include <vector>

#include "../Core/SceneGraph.h"
#include "../Core/Light.h"
#include "../Engine/IAL/I_ResourceFactory.h"
#include "../Engine/IAL/I_DebugUI.h"

#include "nclgl/Vector3.h"

namespace Engine::IAL {
    class I_Shader;
    class I_Texture;
    class I_Mesh;
    class I_FrameBuffer;
}

class PostProcessing;
class Camera;
class Water;

class Renderer {
public:
    Renderer(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
             const std::shared_ptr<SceneGraph>& sceneGraph,
             const std::shared_ptr<Camera>& camera,
                const std::shared_ptr<Engine::IAL::I_DebugUI>& debugUI,
             int width,
             int height);

    void Render();
    void SetWater(const std::shared_ptr<Water>& water);

private:
    void RenderSkybox(const Matrix4& view, const Matrix4& projection);
    void RenderScenePass(const Matrix4& view,
                         const Matrix4& projection,
                         const Vector3& cameraPosition,
                         bool skipWaterNode);
    void RenderWaterSurface(const Matrix4& view,
                            const Matrix4& projection,
                            const Vector3& cameraPosition);
    void RenderReflectionPass(const Matrix4& projection,
                              const Vector3& cameraPosition,
                              float cameraYaw,
                              float cameraPitch);
    void RenderRefractionPass(const Matrix4& view,
                              const Matrix4& projection,
                              const Vector3& cameraPosition);
    void RenderDebugUI();

    std::shared_ptr<Engine::IAL::I_ResourceFactory> m_factory;
    std::shared_ptr<SceneGraph> m_sceneGraph;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Engine::IAL::I_DebugUI> m_debugUI;
    std::vector<std::shared_ptr<SceneNode>> m_renderQueue;
    std::shared_ptr<PostProcessing> m_postProcessing;
    std::shared_ptr<Engine::IAL::I_Shader> m_sceneShader;
    std::shared_ptr<Engine::IAL::I_Shader> m_postShader;
    std::shared_ptr<Engine::IAL::I_Shader> m_terrainShader;
    std::shared_ptr<Engine::IAL::I_Shader> m_skyboxShader;
    std::shared_ptr<Engine::IAL::I_Shader> m_waterShader;
    std::shared_ptr<Engine::IAL::I_Texture> m_skyboxTexture;
    std::shared_ptr<Engine::IAL::I_Mesh> m_skyboxMesh;
    std::shared_ptr<Engine::IAL::I_FrameBuffer> m_waterReflectionFBO;
    std::shared_ptr<Engine::IAL::I_FrameBuffer> m_waterRefractionFBO;
    std::shared_ptr<Water> m_water;
    Light m_directionalLight;
    Vector3 m_sceneColour;
    float m_specularPower;
    float m_nearPlane;
    float m_farPlane;
    int m_surfaceWidth;
    int m_surfaceHeight;
};