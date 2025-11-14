/**
 * @file Renderer.h
 * @brief 声明负责遍历场景图并提交绘制命令的 Renderer 类。
 * @details
 * Renderer 持有资源工厂引用、场景图指针与相机实例，通过 CollectRenderableNodes 收集节点，
 * 在 Render 函数中遍历并调用 I_Mesh::Draw()。Day11 阶段增加了天空盒与光照支持：
 * 在渲染场景几何之前绘制 cubemap 天空盒，并在地形着色器中注入 Blinn-Phong 光照所需的光源与相机参数。
 * Day12 进一步扩展了渲染流程，引入水体节点的递归渲染：在主场景绘制前先渲染反射与折射帧缓冲，
 * 随后使用专用水面着色器将两个纹理组合成最终的水体效果。Day15 则在后期处理中加入过渡着色器，
 * Renderer 可通过 SetTransitionState 将计时进度传递给 PostProcessing，驱动全屏过渡动画。
 */
#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include <array>

#include "../Core/SceneGraph.h"
#include "../Core/Light.h"
#include "../Engine/IAL/I_ResourceFactory.h"
#include "../Engine/IAL/I_DebugUI.h"
#include "../Engine/IAL/I_AnimatedMesh.h"
#include "ShadowMap.h"

#include "nclgl/Matrix4.h"
#include "nclgl/Vector3.h"
#include "nclgl/Vector4.h"

namespace Engine::IAL {
    class I_Shader;
    class I_Texture;
    class I_Mesh;
    class I_FrameBuffer;
    struct PBRMaterial;
    enum class AlphaMode;
    class I_Heightmap;
}

class GrassField;

class PostProcessing;
class Camera;
class Water;
class RainSystem;


class Renderer {
public:
    enum class RenderDebugMode {
        Standard,
        Wireframe,
        Normal,
        Depth,
        Bloom
    };

    enum class ViewLayoutMode {
        Single,
        Quad
    };
    Renderer(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
             const std::shared_ptr<SceneGraph>& sceneGraph,
             const std::shared_ptr<Camera>& camera,
             const std::shared_ptr<Engine::IAL::I_DebugUI>& debugUI,
             int width,
             int height);

    ~Renderer();

    void Render(float deltaTime);
    void SetWater(const std::shared_ptr<Water>& water);
    void SetSkyboxTexture(const std::shared_ptr<Engine::IAL::I_Texture>& texture);
    void SetSceneColour(const Vector3& colour);
    void SetDirectionalLight(const Light& light);
    void SetTransitionState(bool enabled, float progress);
    void SetTerrainHeightmap(const std::shared_ptr<Engine::IAL::I_Heightmap>& heightmap);
    void SetGrassEnabled(bool enabled);
    void SetGrassBaseTexture(const std::shared_ptr<Engine::IAL::I_Texture>& texture);
    bool IsGrassEnabled() const { return m_grassEnabled; }
    void SetRainEnabled(bool enabled);
    bool IsRainEnabled() const { return m_rainEnabled; }

    void SetDefaultViewMode(RenderDebugMode mode);
    RenderDebugMode GetDefaultViewMode() const { return m_defaultViewMode; }
    void ToggleMultiViewLayout();
    void OnSurfaceResized(int width, int height);
    ViewLayoutMode GetViewLayout() const { return m_viewLayout; }

private:
    void RenderSceneForShadowMap(const Matrix4& lightViewProjection,
                                 bool skipWaterNode);
    void RenderSkybox(const Matrix4& view, const Matrix4& projection);
    void RenderScenePass(const Matrix4& view,
                         const Matrix4& projection,
                         const Vector3& cameraPosition,
                         bool skipWaterNode,
                         RenderDebugMode mode,
                         const Vector4* clipPlane = nullptr);
    void RenderWaterSurface(const Matrix4& view,
                            const Matrix4& projection,
                            const Vector3& cameraPosition,
                            RenderDebugMode mode);
    void RenderReflectionPass(const Matrix4& projection,
                               const Vector3& cameraPosition,
                               float cameraYaw,
                               float cameraPitch);
    void RenderRefractionPass(const Matrix4& view,
                               const Matrix4& projection,
                               const Vector3& cameraPosition);
    void RenderRain(const Matrix4& view,
                    const Matrix4& projection,
                    const Vector3& cameraPosition,
                    float cameraYaw,
                    float cameraPitch,
                    RenderDebugMode mode);
    void RenderDebugUI();
    void RenderGrass(const Matrix4& view,
                     const Matrix4& projection,
                     const Vector3& cameraPosition,
                     RenderDebugMode mode);
    void UpdateAnimatedMeshes(float deltaTime);
    float GetTerrainExtent() const;
    Vector3 GetFogColor() const;
    float GetFogDensity() const;
    void UpdateViewRangeFromTerrain();
    void BindBonePalette(const std::vector<Matrix4>& bones, int boneCount);
    void UnbindBonePalette();
    void EnsureBoneBufferCapacity(std::size_t requiredCount);
    Engine::IAL::PBRMaterial ResolveMaterial(const std::shared_ptr<SceneNode>& node,
                                             const std::shared_ptr<Engine::IAL::I_Mesh>& mesh) const;
    static int ToAlphaModeValue(Engine::IAL::AlphaMode mode);
    void RenderSingleView(float deltaTime);
    void RenderQuadView(float deltaTime);
    void RenderViewInternal(const std::shared_ptr<Camera>& camera,
                            RenderDebugMode mode,
                            int viewportX,
                            int viewportY,
                            int viewportWidth,
                            int viewportHeight,
                            bool allowTransition);
    int ToShaderDebugMode(RenderDebugMode mode) const;
    void ApplyPolygonMode(RenderDebugMode mode, int& previousFront, int& previousBack) const;
    void RestorePolygonMode(RenderDebugMode mode, int previousFront, int previousBack) const;
    void UpdateSplitViewCameras();
    Vector3 GetSceneFocusPoint() const;

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
    std::shared_ptr<Engine::IAL::I_Shader> m_shadowShader;
    std::shared_ptr<Engine::IAL::I_Shader> m_skinnedShader;
    std::shared_ptr<Engine::IAL::I_Texture> m_skyboxTexture;
    std::shared_ptr<Engine::IAL::I_Mesh> m_skyboxMesh;
    std::shared_ptr<Engine::IAL::I_FrameBuffer> m_waterReflectionFBO;
    std::shared_ptr<Engine::IAL::I_FrameBuffer> m_waterRefractionFBO;
    std::shared_ptr<ShadowMap> m_shadowMap;
    std::shared_ptr<Water> m_water;
    Light m_directionalLight;
    Vector3 m_sceneColour;
    float m_specularPower;
    float m_nearPlane;
    float m_farPlane;
    int m_surfaceWidth;
    int m_surfaceHeight;
    bool m_transitionEnabled;
    float m_transitionProgress;
    Matrix4 m_shadowMatrix;
    Matrix4 m_reflectionViewProj;
    float m_shadowStrength;
    unsigned int m_bonePaletteBuffer;
    std::size_t m_boneCapacity;
    float m_environmentIntensity;
    float m_environmentMaxLod;
    std::shared_ptr<Engine::IAL::I_Heightmap> m_activeHeightmap;
    std::unique_ptr<GrassField> m_grassField;
    std::unique_ptr<RainSystem> m_rainSystem;
    float m_timeAccumulator;
    std::shared_ptr<Engine::IAL::I_Texture> m_grassBaseTextureOverride;
    bool m_grassEnabled;
    bool m_rainEnabled;
    ViewLayoutMode m_viewLayout;
    RenderDebugMode m_defaultViewMode;
    std::array<std::shared_ptr<Camera>, 3> m_splitCameras;
};