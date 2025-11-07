/**
* @file B_Factory.cpp
 * @brief 轨道 B (NCLGL_Impl) 的资源工厂接口实现源文件。
 *
 * 本文件实现了 B_Factory 类。
 * 在 Day 2 阶段，所有工厂方法均为返回 nullptr 的空壳实现，
 * 旨在满足 main.cpp 中的依赖注入编译需求。
 */
#include "B_Factory.h"

namespace NCLGL_Impl {

    B_Factory::B_Factory() {
    }

    B_Factory::~B_Factory() {
    }

    std::shared_ptr<Engine::IAL::I_Shader> B_Factory::CreateShader(
        const std::string& vPath,
        const std::string& fPath,
        const std::string& gPath) {
        return nullptr;
    }

    std::shared_ptr<Engine::IAL::I_Mesh> B_Factory::LoadMesh(const std::string& path) {
        return nullptr;
    }

    std::shared_ptr<Engine::IAL::I_Texture> B_Factory::LoadTexture(
        const std::string& path, bool repeat) {
        return nullptr;
    }

    std::shared_ptr<Engine::IAL::I_Texture> B_Factory::LoadCubemap(
        const std::string& negx, const std::string& posx,
        const std::string& negy, const std::string& posy,
        const std::string& negz, const std::string& posz) {
        return nullptr;
    }

    std::shared_ptr<Engine::IAL::I_Heightmap> B_Factory::LoadHeightmap(
        const std::string& path, const Vector3& scale) {
        return nullptr;
    }

    std::shared_ptr<Engine::IAL::I_Mesh> B_Factory::CreateQuad() {
        return nullptr;
    }

    std::shared_ptr<Engine::IAL::I_FrameBuffer> B_Factory::CreateShadowFBO(
        int width, int height) {
        return nullptr;
    }

    std::shared_ptr<Engine::IAL::I_FrameBuffer> B_Factory::CreatePostProcessFBO(
        int width, int height) {
        return nullptr;
    }

    std::shared_ptr<Engine::IAL::I_AnimatedMesh> B_Factory::LoadAnimatedMesh(
        const std::string& path,
        const std::string& animPathOrName) {
        return nullptr;
    }

}