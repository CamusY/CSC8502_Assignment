/**
 * @file B_Factory.h
 * @brief 轨道 B (NCLGL_Impl) 的资源工厂接口实现声明。
 *
 * 本文件定义了 B_Factory 类，它是 Engine::IAL::I_ResourceFactory 接口在 nclgl 框架下的具体实现。
 * 作为轨道 B 的核心工厂，它负责创建所有 nclgl 实现的渲染对象（Mesh, Shader, Texture 等）。
 *
 * 前(Day5)仅完成帧缓冲区相关接口：
 * - CreateShadowFBO / CreatePostProcessFBO: 返回配置好的 B_FrameBuffer。
 * 其余资源接口将在 Day6/Day7 中补全，目前均返回 nullptr，以满足暂存流程：
 *
 * B_Factory 类 (NCLGL_Impl::B_Factory):
 * 继承自 Engine::IAL::I_ResourceFactory 纯虚接口。
 *
 * 成员函数 (全部为 I_ResourceFactory 接口的实现):
 * CreateShader: 创建并返回包装了 nclgl::Shader 的 B_Shader。
 * LoadMesh: 加载并返回包装了 nclgl::Mesh 的 B_Mesh。
 * LoadTexture: 加载并返回包装了 OpenGL 纹理 ID 的 B_Texture。
 * LoadCubemap: 加载立方体贴图并返回 B_Texture。
 * LoadHeightmap: 加载 RAW 高度图数据并返回 B_Heightmap。
 * CreateQuad: 创建一个用于后处理的全屏四边形 B_Mesh。
 * CreateShadowFBO: 创建仅包含深度附件的 B_FrameBuffer（禁用颜色附件，适用于阴影映射）。
 * CreatePostProcessFBO: 创建同时包含颜色/深度附件的 B_FrameBuffer（适用于后处理）。
 * LoadAnimatedMesh: 加载并返回包装了 Mesh 和 MeshAnimation 的 B_AnimatedMesh。
 */
#pragma once
#include "IAL/I_ResourceFactory.h"

namespace NCLGL_Impl {

    class B_Factory : public Engine::IAL::I_ResourceFactory {
    public:
        B_Factory();
        ~B_Factory() override;

        std::shared_ptr<Engine::IAL::I_Shader> CreateShader(
            const std::string& vPath,
            const std::string& fPath,
            const std::string& gPath) override;

        std::shared_ptr<Engine::IAL::I_Mesh> LoadMesh(const std::string& path) override;

        std::shared_ptr<Engine::IAL::I_Texture> LoadTexture(
            const std::string& path, bool repeat) override;

        std::shared_ptr<Engine::IAL::I_Texture> LoadCubemap(
            const std::string& negx, const std::string& posx,
            const std::string& negy, const std::string& posy,
            const std::string& negz, const std::string& posz) override;

        std::shared_ptr<Engine::IAL::I_Heightmap> LoadHeightmap(
            const std::string& path, const Vector3& scale) override;

        std::shared_ptr<Engine::IAL::I_Mesh> CreateQuad() override;

        std::shared_ptr<Engine::IAL::I_FrameBuffer> CreateShadowFBO(
            int width, int height) override;

        std::shared_ptr<Engine::IAL::I_FrameBuffer> CreatePostProcessFBO(
            int width, int height) override;

        std::shared_ptr<Engine::IAL::I_AnimatedMesh> LoadAnimatedMesh(
            const std::string& path,
            const std::string& animPathOrName) override;
    };

}