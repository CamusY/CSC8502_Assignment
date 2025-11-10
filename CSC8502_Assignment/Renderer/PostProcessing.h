/**
* @file PostProcessing.h
 * @brief 声明后期处理管线管理类。
 *
 * 本模块封装了 Day6 所需的最小后期处理骨架：
 * - 通过资源工厂创建后期 FBO 与全屏四边形网格；
 * - 提供 BeginCapture/EndCapture 以驱动“场景 -> FBO”渲染；
 * - 提供 Present 以在当前阶段将 FBO 内容直通输出到默认帧缓冲，
 *   并记录渲染目标切换日志，满足 NFR-11.TEX-FBO 的观测性要求；
 * - 保留 GetSceneTexture 接口，方便后续在 Day15 绑定过渡着色器。
 */
#pragma once

#include <memory>

#include "../Engine/IAL/I_ResourceFactory.h"

namespace Engine::IAL {
    class I_FrameBuffer;
    class I_Texture;
    class I_Mesh;
}

class PostProcessing {
public:
    PostProcessing(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
                   int width,
                   int height);

    void Resize(int width, int height);

    void BeginCapture();
    void EndCapture();
    void Present();

    std::shared_ptr<Engine::IAL::I_Texture> GetSceneTexture() const;

private:
    void RecreateResources(int width, int height);

    std::shared_ptr<Engine::IAL::I_ResourceFactory> m_factory;
    std::shared_ptr<Engine::IAL::I_FrameBuffer> m_frameBuffer;
    std::shared_ptr<Engine::IAL::I_Mesh> m_fullscreenQuad;
    int m_width;
    int m_height;
};