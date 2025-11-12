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
        struct TextureSource {
        TextureSource() = default;
        ~TextureSource() {
            if (data) {
                free(data);
                data = nullptr;
            }
        }

        TextureSource(const TextureSource&) = delete;
        TextureSource& operator=(const TextureSource&) = delete;

        TextureSource(TextureSource&& other) noexcept
            : data(other.data)
            , width(other.width)
            , height(other.height)
            , channels(other.channels)
            , flags(other.flags) {
            other.data = nullptr;
            other.width = 0;
            other.height = 0;
            other.channels = 0;
            other.flags = 0;
        }

        TextureSource& operator=(TextureSource&& other) noexcept {
            if (this != &other) {
                if (data) {
                    free(data);
                }
                data = other.data;
                width = other.width;
                height = other.height;
                channels = other.channels;
                flags = other.flags;
                other.data = nullptr;
                other.width = 0;
                other.height = 0;
                other.channels = 0;
                other.flags = 0;
            }
            return *this;
        }

        char* data = nullptr;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t channels = 0;
        uint32_t flags = 0;
    };

    bool DecodeTexture(const std::string& path, TextureSource& out) {
        if (path.empty()) {
            return false;
        }
        if (!OGLTexture::LoadTexture(path, out.data, out.width, out.height, out.channels, out.flags)) {
            return false;
        }
        return out.data != nullptr && out.width > 0 && out.height > 0;
    }

    GLuint UploadTexture2D(const TextureSource& source, bool repeat) {
        if (!source.data || source.width == 0 || source.height == 0) {
            return 0;
        }

        GLuint textureID = 0;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        const GLenum format = source.channels >= 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     source.channels >= 4 ? GL_RGBA8 : GL_RGB8,
                     static_cast<GLsizei>(source.width),
                     static_cast<GLsizei>(source.height),
                     0,
                     format,
                     GL_UNSIGNED_BYTE,
                     source.data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        const GLint wrapMode = repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
        return textureID;
    }

    GLuint UploadCubemap(const std::array<TextureSource, 6>& faces) {
        if (!faces[0].data) {
            return 0;
        }

        GLuint textureID = 0;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        const GLenum format = faces[0].channels >= 4 ? GL_RGBA : GL_RGB;

        for (size_t i = 0; i < faces.size(); ++i) {
            const auto& face = faces[i];
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i),
                         0,
                         faces[i].channels >= 4 ? GL_RGBA8 : GL_RGB8,
                         static_cast<GLsizei>(face.width),
                         static_cast<GLsizei>(face.height),
                         0,
                         format,
                         GL_UNSIGNED_BYTE,
                         face.data);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        return textureID;
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
        TextureSource source;
        if (!DecodeTexture(path, source)) {
            std::cerr << "[B_Factory] Texture decode failed for " << path << std::endl;
            return nullptr;
        }

        const GLuint textureID = UploadTexture2D(source, repeat);
        if (textureID == 0) {
            std::cerr << "[B_Factory] Texture upload failed for " << path << std::endl;
            return nullptr;
        }

        std::cerr << "[B_Factory] Texture loaded: " << path
                  << " (" << source.width << "x" << source.height
                  << ", repeat=" << (repeat ? "true" : "false") << ")\n";
        return std::make_shared<B_Texture>(textureID, Engine::IAL::TextureType::Texture2D);
    }

    std::shared_ptr<Engine::IAL::I_Texture> B_Factory::LoadCubemap(
        const std::string& negx, const std::string& posx,
        const std::string& negy, const std::string& posy,
        const std::string& negz, const std::string& posz) {
        std::array<TextureSource, 6> faces;
        const std::array<std::string, 6> orderedPaths = { posx, negx, posy, negy, posz, negz };

        for (size_t i = 0; i < orderedPaths.size(); ++i) {
            if (!DecodeTexture(orderedPaths[i], faces[i])) {
                std::cerr << "[B_Factory] Cubemap decode failed for " << orderedPaths[i] << std::endl;
                return nullptr;
            }
            if (i > 0) {
                if (faces[i].width != faces[0].width || faces[i].height != faces[0].height) {
                    std::cerr << "[B_Factory] Cubemap size mismatch between " << orderedPaths[0]
                              << " and " << orderedPaths[i] << std::endl;
                    return nullptr;
                }
            }
        }

        const GLuint textureID = UploadCubemap(faces);
        if (textureID == 0) {
            std::cerr << "[B_Factory] Cubemap upload failed" << std::endl;
            return nullptr;
        }

        std::cerr << "[B_Factory] Cubemap loaded: "
                  << posx << ", " << negx << ", " << posy << ", " << negy << ", " << posz << ", " << negz
                  << " (" << faces[0].width << "x" << faces[0].height << ")\n";
        return std::make_shared<B_Texture>(textureID, Engine::IAL::TextureType::CubeMap);
    }

    std::shared_ptr<Engine::IAL::I_Heightmap> B_Factory::LoadHeightmap(
        const std::string& path, const Vector3& scale) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            std::cerr << "[B_Factory] Failed to open heightmap " << path << std::endl;
            return nullptr;
        }

        std::vector<unsigned char> samples((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if (samples.empty()) {
            std::cerr << "[B_Factory] Heightmap file empty: " << path << std::endl;
            return nullptr;
        }

        const double sampleCount = static_cast<double>(samples.size());
        const size_t dimension = static_cast<size_t>(std::sqrt(sampleCount));
        if (dimension * dimension != samples.size() || dimension < 2) {
            std::cerr << "[B_Factory] Heightmap dimensions invalid: " << path
                      << " (sample count=" << samples.size() << ")" << std::endl;
            return nullptr;
        }

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