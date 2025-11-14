/**
* @file PostProcessing.h
 * @brief 声明后期处理管线管理类。
 *
 * 本模块封装了 Day6 所需的最小后期处理骨架：
 * - 通过资源工厂创建后期 FBO 与全屏四边形网格；
 * - 提供 BeginCapture/EndCapture 以驱动“场景 -> FBO”渲染；
 * - 提供 Present 以在当前阶段将 FBO 内容输出到默认帧缓冲，
 *   并记录渲染目标切换日志，满足 NFR-11.TEX-FBO 的观测性要求；
 * - Day15 增强：内部维护直通与过渡两套着色器，可接受 `float timer`
 *   Uniform 以驱动全屏过渡特效。
 */
#pragma once

#include <memory>

#include "../Engine/IAL/I_ResourceFactory.h"

namespace Engine::IAL {
    class I_FrameBuffer;
    class I_Texture;
    class I_Mesh;
    class I_Shader;
}

class PostProcessing {
public:
    enum class OutputMode {
        ToneMapped,
        RawScene,
        BloomOnly
    };

    PostProcessing(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
                   int width,
                   int height);

    void Resize(int width, int height);

    void BeginCapture();
    void EndCapture();
    void PresentToViewport(OutputMode mode,
                           bool transitionEnabled,
                           float timer,
                           int viewportX,
                           int viewportY,
                           int viewportWidth,
                           int viewportHeight);

    std::shared_ptr<Engine::IAL::I_Texture> GetSceneTexture() const;
    std::shared_ptr<Engine::IAL::I_Texture> GetBloomTexture() const;
    void SetExposure(float exposure);

private:
    void RecreateResources(int width, int height);
    void ProcessBloom();

    std::shared_ptr<Engine::IAL::I_ResourceFactory> m_factory;
    std::shared_ptr<Engine::IAL::I_FrameBuffer> m_frameBuffer;
    std::shared_ptr<Engine::IAL::I_FrameBuffer> m_brightFrameBuffer;
    std::shared_ptr<Engine::IAL::I_FrameBuffer> m_pingPongBuffers[2];
    std::shared_ptr<Engine::IAL::I_Mesh> m_fullscreenQuad;
    std::shared_ptr<Engine::IAL::I_Shader> m_passthroughShader;
    std::shared_ptr<Engine::IAL::I_Shader> m_transitionShader;
    std::shared_ptr<Engine::IAL::I_Shader> m_brightShader;
    std::shared_ptr<Engine::IAL::I_Shader> m_blurShader;
    int m_width;
    int m_height;
    bool m_bloomDirty;
    float m_exposure;
    std::shared_ptr<Engine::IAL::I_Texture> m_cachedBloomTexture;
};