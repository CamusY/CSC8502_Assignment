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
#include "B_Mesh.h"
#include "B_Shader.h"

#include "nclgl/Mesh.h"
#include "nclgl/Vector2.h"
#include "nclgl/Vector3.h"
#include "nclgl/Extra/GLTFLoader.h"

#include <iostream>
#include <algorithm>
#include <cctype>
#include <memory>
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
        auto shader = std::make_unique<::Shader>(vPath, fPath, gPath);
        if (!shader->LoadSuccess()) {
            std::cerr << "[B_Factory] Failed to create shader from paths: "
                      << vPath << ", " << fPath << ", " << gPath << "\n";
            return nullptr;
        }
        std::cerr << "[B_Factory] Created shader from paths: "
                  << vPath << ", " << fPath << ", " << gPath << "\n";
        return std::make_shared<B_Shader>(shader.release());
    }

    std::shared_ptr<Engine::IAL::I_Mesh> B_Factory::LoadMesh(const std::string& path) {
        if (path.empty()) {
            return nullptr;
        }

        std::string extension;
        const std::size_t dot = path.find_last_of('.');
        if (dot != std::string::npos) {
            extension = path.substr(dot);
            std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char ch) {
                return static_cast<char>(std::tolower(ch));
            });
        }

        try {
            if (extension == ".gltf" || extension == ".glb") {
                GLTFScene scene;
                if (!GLTFLoader::Load(path, scene) || scene.meshes.empty()) {
                    std::cerr << "[B_Factory] GLTF load failed for " << path << std::endl;
                    return nullptr;
                }
                auto mesh = scene.meshes.front();
                std::cerr << "[B_Factory] GLTF mesh loaded: " << path << std::endl;
                return std::make_shared<B_Mesh>(mesh);
            }

            std::shared_ptr<::Mesh> mesh(::Mesh::LoadFromMeshFile(path));
            if (!mesh) {
                std::cerr << "[B_Factory] Mesh load failed for " << path << std::endl;
                return nullptr;
            }
            std::cerr << "[B_Factory] Mesh loaded: " << path << std::endl;
            return std::make_shared<B_Mesh>(mesh);
        }
        catch (const std::exception& ex) {
            std::cerr << "[B_Factory] Exception while loading mesh " << path << ": " << ex.what() << std::endl;
            return nullptr;
        }
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
    
    namespace {
        class FullscreenQuadMesh final : public ::Mesh {
        public:
            FullscreenQuadMesh() {
                numVertices = 4;
                numIndices = 6;
                type = GL_TRIANGLES;

                vertices = new Vector3[numVertices];
                textureCoords = new Vector2[numVertices];
                indices = new unsigned int[numIndices];

                vertices[0] = Vector3(-1.0f, -1.0f, 0.0f);
                vertices[1] = Vector3(1.0f, -1.0f, 0.0f);
                vertices[2] = Vector3(1.0f, 1.0f, 0.0f);
                vertices[3] = Vector3(-1.0f, 1.0f, 0.0f);

                textureCoords[0] = Vector2(0.0f, 0.0f);
                textureCoords[1] = Vector2(1.0f, 0.0f);
                textureCoords[2] = Vector2(1.0f, 1.0f);
                textureCoords[3] = Vector2(0.0f, 1.0f);

                indices[0] = 0;
                indices[1] = 1;
                indices[2] = 2;
                indices[3] = 0;
                indices[4] = 2;
                indices[5] = 3;

                BufferData();
            }
        };
    }

    std::shared_ptr<Engine::IAL::I_Mesh> B_Factory::CreateQuad() {
        std::shared_ptr<::Mesh> quadMesh(new FullscreenQuadMesh());
        std::cerr << "[B_Factory] Created fullscreen quad mesh" << "\n";
        return std::make_shared<B_Mesh>(quadMesh);
    }

    std::shared_ptr<Engine::IAL::I_FrameBuffer> B_Factory::CreateShadowFBO(
        int width, int height) {
        auto fbo = std::make_shared<B_FrameBuffer>(width, height, false);
        std::cerr << "[B_Factory] Shadow FBO layout "
                  << BuildLayoutDescription(fbo) << "; default FBO assumed Color8/Depth24. Size: "
                  << width << "x" << height << "\n";
        return fbo;
    }

    std::shared_ptr<Engine::IAL::I_FrameBuffer> B_Factory::CreatePostProcessFBO(
        int width, int height) {
        auto fbo = std::make_shared<B_FrameBuffer>(width, height, true);
        std::cerr << "[B_Factory] PostProcess FBO layout "
                  << BuildLayoutDescription(fbo) << "; default FBO assumed Color8/Depth24. Size: "
                  << width << "x" << height << "\n";
        return fbo;
    }

    std::shared_ptr<Engine::IAL::I_AnimatedMesh> B_Factory::LoadAnimatedMesh(
        const std::string& path,
        const std::string& animPathOrName) {
        return nullptr;
    }

}