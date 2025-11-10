/**
* @file B_Factory.cpp
 * @brief 轨道 B (NCLGL_Impl) 的资源工厂接口实现源文件。
 *
 * 本文件实现了 B_Factory 类。该类继承自 I_ResourceFactory，
 * 提供了用于创建和加载各种图形资源（如着色器、网格、纹理、高度图等）的具体方法。
 * 当前(Day5)实现中，大部分方法返回空指针，只有帧缓冲区创建方法返回了具体的 B_FrameBuffer 实例。
 */
#include "B_Factory.h"
#include "B_FrameBuffer.h"

#include <iostream>
#include <string>

namespace {
    using AttachmentFormat = NCLGL_Impl::AttachmentFormat;

    std::string AttachmentFormatToString(AttachmentFormat format) {
        switch (format) {
        case AttachmentFormat::None: return "None";
        case AttachmentFormat::Color8: return "Color8";
        case AttachmentFormat::Color16F: return "Color16F";
        case AttachmentFormat::Depth24: return "Depth24";
        case AttachmentFormat::Depth32F: return "Depth32F";
        }
        return "Unknown";
    }

    std::string BuildLayoutDescription(const std::shared_ptr<NCLGL_Impl::B_FrameBuffer>& fbo) {
        const bool hasColor = fbo->GetColorFormat() != AttachmentFormat::None;
        const AttachmentFormat depthFormat = fbo->GetDepthFormat();
        const std::string depthStr = AttachmentFormatToString(depthFormat);

        if (!hasColor) {
            if (depthFormat == AttachmentFormat::None) {
                return "Empty (None)";
            }
            return "Depth-only (" + depthStr + ")";
        }

        std::string description = "Color+Depth (" + AttachmentFormatToString(fbo->GetColorFormat());
        if (depthFormat != AttachmentFormat::None) {
            description += "/" + depthStr;
        }
        description += ")";
        return description;
    }
}

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
        auto fbo = std::make_shared<B_FrameBuffer>(width, height, false);
        std::cerr << "[B_Factory] Shadow FBO layout "
                  << BuildLayoutDescription(fbo) << "; default FBO assumed Color8/Depth24. Size: "
                  << width << "x" << height << std::endl;
        return fbo;
    }

    std::shared_ptr<Engine::IAL::I_FrameBuffer> B_Factory::CreatePostProcessFBO(
        int width, int height) {
        auto fbo = std::make_shared<B_FrameBuffer>(width, height, true);
        std::cerr << "[B_Factory] PostProcess FBO layout "
                  << BuildLayoutDescription(fbo) << "; default FBO assumed Color8/Depth24. Size: "
                  << width << "x" << height << std::endl;
        return fbo;
    }

    std::shared_ptr<Engine::IAL::I_AnimatedMesh> B_Factory::LoadAnimatedMesh(
        const std::string& path,
        const std::string& animPathOrName) {
        return nullptr;
    }

}