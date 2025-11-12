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
#include "B_Heightmap.h"
#include "B_Mesh.h"
#include "B_Shader.h"
#include "B_Texture.h"

#include "nclgl/Mesh.h"
#include "nclgl/Vector2.h"
#include "nclgl/Vector3.h"
#include "nclgl/Extra/GLTFLoader.h"
#include "nclgl/Extra/OGLTexture.h"

#include <glad/glad.h>

#include <iostream>
#include <array>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <nclgl/Extra/stb/stb_image.h>


namespace {
    using AttachmentFormat = NCLGL_Impl::AttachmentFormat;

    std::string AttachmentFormatToString(AttachmentFormat format) {
        switch (format) {
        case AttachmentFormat::None:
            return "None";
        case AttachmentFormat::Color8:
            return "Color8";
        case AttachmentFormat::Color16F:
            return "Color16F";
        case AttachmentFormat::Depth24:
            return "Depth24";
        case AttachmentFormat::Depth32F:
            return "Depth32F";
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

    struct TextureDescriptor {
        std::string path;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t channels = 0;
    };

    bool DecodeTexture(const std::string& path, TextureDescriptor& out) {
        if (path.empty()) {
            return false;
        }
        char* data = nullptr;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t channels = 0;
        uint32_t flags = 0;
        if (!OGLTexture::LoadTexture(path, data, width, height, channels, flags)) {
            return false;
        }
        free(data);
        out.path = path;
        out.width = width;
        out.height = height;
        out.channels = channels;
        return out.width > 0 && out.height > 0;
    }

    UniqueOGLTexture UploadTexture2D(const TextureDescriptor& source, bool repeat) {
        if (source.path.empty()) {
            return nullptr;
        }
        UniqueOGLTexture texture = OGLTexture::TextureFromFile(source.path);
        if (!texture) {
            return nullptr;
        }
        glBindTexture(GL_TEXTURE_2D, texture->GetObjectID());
        const GLint wrapMode = repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        return texture;
    }

    bool DecodeCubemap(const std::array<std::string, 6>& paths, std::array<TextureDescriptor, 6>& descriptors) {
        for (size_t i = 0; i < paths.size(); ++i) {
            if (!DecodeTexture(paths[i], descriptors[i])) {
                std::cerr << "[B_Factory] Cubemap decode failed for " << paths[i] << "\n";
                return false;
            }
            if (i > 0) {
                if (descriptors[i].width != descriptors[0].width || descriptors[i].height != descriptors[0].height) {
                    std::cerr << "[B_Factory] Cubemap size mismatch between " << paths[0]
                        << " and " << paths[i] << "\n";
                    return false;
                }
            }
        }
        return true;
    }

    UniqueOGLTexture UploadCubemap(const std::array<std::string, 6>& orderedPaths) {
        UniqueOGLTexture texture = OGLTexture::LoadCubemap(orderedPaths[0],
                                                           orderedPaths[1],
                                                           orderedPaths[2],
                                                           orderedPaths[3],
                                                           orderedPaths[4],
                                                           orderedPaths[5]);
        if (!texture) {
            return nullptr;
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture->GetObjectID());
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        return texture;
    }
    std::shared_ptr<Engine::IAL::I_Texture> CreateFallbackCubemap() {
        unsigned int cubemapID = 0;
        glGenTextures(1, &cubemapID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

        const std::array<std::array<unsigned char, 3>, 6> colors = {{
            {128, 178, 255},
            {128, 178, 255},
            {255, 200, 140},
            {255, 200, 140},
            {110, 160, 220},
            {110, 160, 220}
        }};

        for (size_t i = 0; i < colors.size(); ++i) {
            const auto& color = colors[i];
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i),
                         0,
                         GL_RGB,
                         1,
                         1,
                         0,
                         GL_RGB,
                         GL_UNSIGNED_BYTE,
                         color.data());
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        std::cerr << "[B_Factory] Generated fallback cubemap texture" << "\n";
        return std::make_shared<NCLGL_Impl::B_Texture>(cubemapID, Engine::IAL::TextureType::CubeMap, GL_TEXTURE_CUBE_MAP);
    }

    class HeightmapMesh final : public ::Mesh {
    public:
        HeightmapMesh(const std::vector<unsigned char>& samples, size_t dimension, const Vector3& scale) {
            numVertices = static_cast<GLuint>(dimension * dimension);
            numIndices = static_cast<GLuint>((dimension - 1) * (dimension - 1) * 6);
            type = GL_TRIANGLES;

            vertices = new Vector3[numVertices];
            normals = new Vector3[numVertices];
            textureCoords = new Vector2[numVertices];
            indices = new unsigned int[numIndices];

            auto sampleHeight = [&](int x, int z) {
                x = std::clamp(x, 0, static_cast<int>(dimension) - 1);
                z = std::clamp(z, 0, static_cast<int>(dimension) - 1);
                const size_t index = static_cast<size_t>(z) * dimension + static_cast<size_t>(x);
                const float normalised = static_cast<float>(samples[index]) / 255.0f;
                return normalised * scale.y;
            };

            for (size_t z = 0; z < dimension; ++z) {
                for (size_t x = 0; x < dimension; ++x) {
                    const size_t index = z * dimension + x;
                    const float height = sampleHeight(static_cast<int>(x), static_cast<int>(z));

                    vertices[index] = Vector3(static_cast<float>(x) * scale.x,
                                              height,
                                              static_cast<float>(z) * scale.z);
                    textureCoords[index] = Vector2(
                        dimension > 1 ? static_cast<float>(x) / static_cast<float>(dimension - 1) : 0.0f,
                        dimension > 1 ? static_cast<float>(z) / static_cast<float>(dimension - 1) : 0.0f);
                    normals[index] = Vector3(0.0f, 1.0f, 0.0f);
                }
            }

            for (size_t z = 0; z < dimension; ++z) {
                for (size_t x = 0; x < dimension; ++x) {
                    const size_t index = z * dimension + x;
                    const float hL = sampleHeight(static_cast<int>(x) - 1, static_cast<int>(z));
                    const float hR = sampleHeight(static_cast<int>(x) + 1, static_cast<int>(z));
                    const float hD = sampleHeight(static_cast<int>(x), static_cast<int>(z) - 1);
                    const float hU = sampleHeight(static_cast<int>(x), static_cast<int>(z) + 1);

                    const Vector3 tangentX(2.0f * scale.x, hR - hL, 0.0f);
                    const Vector3 tangentZ(0.0f, hU - hD, 2.0f * scale.z);

                    Vector3 normal = Vector3::Cross(tangentZ, tangentX);
                    normal.Normalise();
                    normals[index] = normal;
                }
            }

            unsigned int indexCursor = 0;
            for (size_t z = 0; z < dimension - 1; ++z) {
                for (size_t x = 0; x < dimension - 1; ++x) {
                    const unsigned int a = static_cast<unsigned int>(z * dimension + x);
                    const unsigned int b = static_cast<unsigned int>(z * dimension + (x + 1));
                    const unsigned int c = static_cast<unsigned int>((z + 1) * dimension + (x + 1));
                    const unsigned int d = static_cast<unsigned int>((z + 1) * dimension + x);

                    indices[indexCursor++] = a;
                    indices[indexCursor++] = b;
                    indices[indexCursor++] = c;
                    indices[indexCursor++] = a;
                    indices[indexCursor++] = c;
                    indices[indexCursor++] = d;
                }
            }

            BufferData();
        }
    };
}

namespace NCLGL_Impl {

    B_Factory::B_Factory() {}

    B_Factory::~B_Factory() {}

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
                    std::cerr << "[B_Factory] GLTF load failed for " << path << "\n";
                    return nullptr;
                }
                auto mesh = scene.meshes.front();
                std::cerr << "[B_Factory] GLTF mesh loaded: " << path << "\n";
                return std::make_shared<B_Mesh>(mesh);
            }

            std::shared_ptr<::Mesh> mesh(::Mesh::LoadFromMeshFile(path));
            if (!mesh) {
                std::cerr << "[B_Factory] Mesh load failed for " << path << "\n";
                return nullptr;
            }
            std::cerr << "[B_Factory] Mesh loaded: " << path << "\n";
            return std::make_shared<B_Mesh>(mesh);
        }
        catch (const std::exception& ex) {
            std::cerr << "[B_Factory] Exception while loading mesh " << path << ": " << ex.what() << "\n";
            return nullptr;
        }
    }

    std::shared_ptr<Engine::IAL::I_Texture> B_Factory::LoadTexture(
           const std::string& path, bool repeat) {
        TextureDescriptor descriptor;
        if (!DecodeTexture(path, descriptor)) {
            std::cerr << "[B_Factory] Texture decode failed for " << path << "\n";
            return nullptr;
        }

        auto texture = UploadTexture2D(descriptor, repeat);
        if (!texture) {
            std::cerr << "[B_Factory] Texture upload failed for " << path << "\n";
            return nullptr;
        }

        std::cerr << "[B_Factory] Texture loaded: " << path
                  << " (" << descriptor.width << "x" << descriptor.height
                  << ", repeat=" << (repeat ? "true" : "false") << ")\n";
        return std::make_shared<B_Texture>(std::move(texture), Engine::IAL::TextureType::Texture2D);
    }

    std::shared_ptr<Engine::IAL::I_Texture> B_Factory::LoadCubemap(
        const std::string& negx, const std::string& posx,
        const std::string& negy, const std::string& posy,
        const std::string& negz, const std::string& posz) {
        const std::array<std::string, 6> orderedPaths = { posx, negx, posy, negy, posz, negz };
        std::array<TextureDescriptor, 6> descriptors;
        if (!DecodeCubemap(orderedPaths, descriptors)) {
            std::cerr << "[B_Factory] Cubemap decode failed or dimensions mismatch, using fallback" << "\n";
            return CreateFallbackCubemap();
        }

        auto texture = UploadCubemap(orderedPaths);
        if (!texture) {
            std::cerr << "[B_Factory] Cubemap upload failed, using fallback" << "\n";
            return CreateFallbackCubemap();
        }

        std::cerr << "[B_Factory] Cubemap loaded: "
                  << posx << ", " << negx << ", " << posy << ", " << negy << ", " << posz << ", " << negz
                  << " (" << descriptors[0].width << "x" << descriptors[0].height << ")\n";
        return std::make_shared<B_Texture>(std::move(texture), Engine::IAL::TextureType::CubeMap);
    }

    std::shared_ptr<Engine::IAL::I_Heightmap> B_Factory::LoadHeightmap(
        const std::string& path, const Vector3& scale) {

        int width = 0;
        int height = 0;
        int channels = 0; // 这个变量不会被使用，但 stbi_load 需要它

        // 关键：使用 stbi_load 加载图像。
        // 我们强制只加载 1 个通道 (grayscale)，因为这是高度图。
        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 1);

        if (!data) {
            std::cerr << "[B_Factory] STB_Image Failed to open heightmap: " << path << std::endl;
            return nullptr;
        }

        // 你的 HeightmapMesh 构造函数 期望一个正方形的维度。
        if (width != height || width < 2) {
            std::cerr << "[B_Factory] Heightmap dimensions invalid: " << path
                << " (dimensions=" << width << "x" << height << ")" << std::endl;
            
            free(data); 
            return nullptr;
        }

        const size_t dimension = static_cast<size_t>(width);
        const size_t pixelCount = dimension * dimension;

        // 将 stbi_load 返回的 C-style 数组 (stbi_uc*) 
        // 复制到 HeightmapMesh 构造函数所需的 std::vector<unsigned char> 中。
        std::vector<unsigned char> samples;
        samples.assign(data, data + pixelCount);

        // 修正：使用 free() 而不是 stbi_free()
        free(data);

        // --- 从这里开始，代码和你原来的版本 完全相同 ---
        try {
            auto* mesh = new HeightmapMesh(samples, dimension, scale);
            std::cerr << "[B_Factory] Heightmap loaded: " << path
                << " (" << dimension << "x" << dimension << ") scale="
                << scale.x << "," << scale.y << "," << scale.z << "\n";
            return std::make_shared<B_Heightmap>(mesh);
        }
        catch (const std::exception& ex) {
            std::cerr << "[B_Factory] Exception constructing heightmap mesh for " << path
                << ": " << ex.what() << std::endl;
        }
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
