/**
* @file B_Factory.cpp
 * @brief 轨道 B (NCLGL_Impl) 的资源工厂接口实现源文件。
 *
 * 本文件实现了 B_Factory 类。该类继承自 I_ResourceFactory，
 * 提供了用于创建和加载各种图形资源（如着色器、网格、纹理、高度图等）的具体方法。
 * 当前(Day5)实现中，大部分方法返回空指针，只有帧缓冲区创建方法返回了具体的 B_FrameBuffer 实例。
 */
#include "B_Factory.h"
#include "B_AnimatedMesh.h"
#include "B_FrameBuffer.h"
#include "B_Heightmap.h"
#include "B_Mesh.h"
#include "B_Shader.h"
#include "B_Texture.h"


#include "nclgl/Mesh.h"
#include "nclgl/MeshAnimation.h"
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
#include <cstring>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <vector>
#include <nclgl/Extra/stb/stb_image.h>


namespace {

    std::string BuildLayoutDescription(const std::shared_ptr<NCLGL_Impl::B_FrameBuffer>& fbo) {
        const bool hasColor = fbo->GetColorFormat() != NCLGL_Impl::AttachmentFormat::None;
        const NCLGL_Impl::AttachmentFormat depthFormat = fbo->GetDepthFormat();
        const std::string depthStr = NCLGL_Impl::AttachmentFormatToString(depthFormat);

        if (!hasColor) {
            if (depthFormat == NCLGL_Impl::AttachmentFormat::None) {
                return "Empty (None)";
            }
            return "Depth-only (" + depthStr + ")";
        }

        std::string description = "Color+Depth (" + NCLGL_Impl::AttachmentFormatToString(fbo->GetColorFormat());
        if (depthFormat != NCLGL_Impl::AttachmentFormat::None) {
            description += "/" + depthStr;
        }
        description += ")";
        return description;
    }

    std::string ToLowerCopy(const std::string& value) {
        std::string result = value;
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        });
        return result;
    }

    std::string ExtractExtension(const std::string& path) {
        const std::size_t dot = path.find_last_of('.');
        if (dot == std::string::npos) {
            return std::string();
        }
        return ToLowerCopy(path.substr(dot));
    }

    bool IsDigits(const std::string& value) {
        if (value.empty()) {
            return false;
        }
        return std::all_of(value.begin(), value.end(), [](unsigned char ch) {
            return std::isdigit(ch) != 0;
        });
    }


    struct TextureDescriptor {
        std::string path;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t channels = 0;
        std::vector<unsigned char> pixels;
    };

    bool DecodeTexture(const std::string& path, TextureDescriptor& out, bool flipVertical) {
        if (path.empty()) {
            return false;
        }
        stbi_set_flip_vertically_on_load(flipVertical ? 1 : 0);
        char* rawData = nullptr;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t channels = 0;
        uint32_t flags = 0;
        if (!OGLTexture::LoadTexture(path, rawData, width, height, channels, flags) || !rawData || width == 0 || height
            == 0) {
            if (rawData) {
                stbi_image_free(rawData);
            }
            stbi_set_flip_vertically_on_load(0);
            std::cerr << "[B_Factory] stb_image decode failed for " << path << "\n";
            return false;
        }
        const size_t dataSize = static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(
            channels);
        out.path = path;
        out.width = width;
        out.height = height;
        out.channels = channels;
        out.pixels.resize(dataSize);
        std::memcpy(out.pixels.data(), rawData, dataSize);
        stbi_image_free(rawData);
        stbi_set_flip_vertically_on_load(0);
        return true;
    }

    GLenum ResolveFormat(uint32_t channels) {
        switch (channels) {
        case 1:
            return GL_RED;
        case 2:
            return GL_RG;
        case 3:
            return GL_RGB;
        case 4:
            return GL_RGBA;
        default:
            return GL_RGBA;
        }
    }

    GLuint ResolveInternalFormat(uint32_t channels) {
        switch (channels) {
        case 1:
            return GL_R8;
        case 2:
            return GL_RG8;
        case 3:
            return GL_SRGB8;
        case 4:
            return GL_SRGB8_ALPHA8;
        default:
            return GL_SRGB8_ALPHA8;
        }
    }

    std::shared_ptr<Engine::IAL::I_Texture> UploadTexture2D(const TextureDescriptor& source, bool repeat) {
        if (source.pixels.empty() || source.width == 0 || source.height == 0) {
            return nullptr;
        }
        GLuint textureID = 0;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        const GLenum format = ResolveFormat(source.channels);
        const GLuint internalFormat = ResolveInternalFormat(source.channels);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     internalFormat,
                     static_cast<GLsizei>(source.width),
                     static_cast<GLsizei>(source.height),
                     0,
                     format,
                     GL_UNSIGNED_BYTE,
                     source.pixels.data());
        glGenerateMipmap(GL_TEXTURE_2D);
        const GLint wrapMode = repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GLint minFilter = 0;
        GLint magFilter = 0;
        GLint wrapS = 0;
        GLint wrapT = 0;
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapS);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapT);
        glBindTexture(GL_TEXTURE_2D, 0);
        std::cerr << "[B_Factory] Texture sampler: MIN=" << NCLGL_Impl::FilterToString(minFilter)
            << ", MAG=" << NCLGL_Impl::FilterToString(magFilter)
            << ", WRAP_S=" << NCLGL_Impl::WrapToString(wrapS)
            << ", WRAP_T=" << NCLGL_Impl::WrapToString(wrapT) << "\n";
        return std::make_shared<NCLGL_Impl::B_Texture>(textureID, Engine::IAL::TextureType::Texture2D);
    }

    bool DecodeCubemap(const std::array<std::string, 6>& paths, std::array<TextureDescriptor, 6>& descriptors) {
        for (size_t i = 0; i < paths.size(); ++i) {
            if (!DecodeTexture(paths[i], descriptors[i], false)) {
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

    std::shared_ptr<Engine::IAL::I_Texture> UploadCubemap(const std::array<TextureDescriptor, 6>& descriptors) {
        GLuint cubemapID = 0;
        glGenTextures(1, &cubemapID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
        for (size_t i = 0; i < descriptors.size(); ++i) {
            const auto& face = descriptors[i];
            if (face.pixels.empty()) {
                continue;
            }
            const GLenum format = ResolveFormat(face.channels);
            const GLuint internalFormat = face.channels == 4
                ? GL_SRGB8_ALPHA8
                : (face.channels == 3 ? GL_SRGB8 : GL_RGBA8);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i),
                         0,
                         internalFormat,
                         static_cast<GLsizei>(face.width),
                         static_cast<GLsizei>(face.height),
                         0,
                         format,
                         GL_UNSIGNED_BYTE,
                         face.pixels.data());
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        GLint minFilter = 0;
        GLint magFilter = 0;
        GLint wrapS = 0;
        GLint wrapT = 0;
        GLint wrapR = 0;
        glGetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, &minFilter);
        glGetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, &magFilter);
        glGetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, &wrapS);
        glGetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, &wrapT);
        glGetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, &wrapR);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        std::cerr << "[B_Factory] Cubemap sampler: MIN=" << NCLGL_Impl::FilterToString(minFilter)
            << ", MAG=" << NCLGL_Impl::FilterToString(magFilter)
            << ", WRAP_S=" << NCLGL_Impl::WrapToString(wrapS)
            << ", WRAP_T=" << NCLGL_Impl::WrapToString(wrapT)
            << ", WRAP_R=" << NCLGL_Impl::WrapToString(wrapR) << "\n";
        return std::make_shared<NCLGL_Impl::B_Texture>(cubemapID, Engine::IAL::TextureType::CubeMap,
                                                       GL_TEXTURE_CUBE_MAP);
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
        return std::make_shared<NCLGL_Impl::B_Texture>(cubemapID, Engine::IAL::TextureType::CubeMap,
                                                       GL_TEXTURE_CUBE_MAP);
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
                const float normalised = static_cast<float>(samples[index]);
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

        const std::string extension = ExtractExtension(path);

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
        if (!DecodeTexture(path, descriptor, true)) {
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
        return texture;
    }

    std::shared_ptr<Engine::IAL::I_Texture> B_Factory::LoadCubemap(
        const std::string& negx, const std::string& posx,
        const std::string& negy, const std::string& posy,
        const std::string& negz, const std::string& posz) {
        const std::array<std::string, 6> orderedPaths = {posx, negx, posy, negy, posz, negz};
        std::array<TextureDescriptor, 6> descriptors;
        if (!DecodeCubemap(orderedPaths, descriptors)) {
            std::cerr << "[B_Factory] Cubemap decode failed or dimensions mismatch, using fallback" << "\n";
            return CreateFallbackCubemap();
        }

        auto texture = UploadCubemap(descriptors);
        if (!texture) {
            std::cerr << "[B_Factory] Cubemap upload failed, using fallback" << "\n";
            return CreateFallbackCubemap();
        }

        std::cerr << "[B_Factory] Cubemap loaded: "
            << posx << ", " << negx << ", " << posy << ", " << negy << ", " << posz << ", " << negz
            << " (" << descriptors[0].width << "x" << descriptors[0].height << ")\n";
        return texture;
    }

    std::shared_ptr<Engine::IAL::I_Heightmap> B_Factory::LoadHeightmap(
        const std::string& path, const Vector3& scale) {
        int width = 0;
        int height = 0;
        int channels = 0;
        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 1);
        if (!data) {
            std::cerr << "[B_Factory] STB_Image Failed to open heightmap: " << path << std::endl;
            return nullptr;
        }
        if (width != height || width < 2) {
            std::cerr << "[B_Factory] Heightmap dimensions invalid: " << path
                << " (dimensions=" << width << "x" << height << ")" << std::endl;
            stbi_image_free(data);
            return nullptr;
        }
        const size_t dimension = static_cast<size_t>(width);
        const size_t pixelCount = dimension * dimension;
        std::vector<unsigned char> samples(pixelCount);
        std::memcpy(samples.data(), data, pixelCount);
        stbi_image_free(data);
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
        if (path.empty()) {
            return nullptr;
        }

        const std::string modelExtension = ExtractExtension(path);

        auto logAnimatedMesh = [](const std::string& source,
                                  const std::shared_ptr<::MeshAnimation>& animation) {
            if (animation) {
                std::cerr << "[B_Factory] Animated mesh loaded: " << source
                    << " (frames=" << animation->GetFrameCount()
                    << ", joints=" << animation->GetJointCount() << ")\n";
            }
        };

        try {
            if (modelExtension == ".gltf" || modelExtension == ".glb") {
                GLTFScene scene;
                if (!GLTFLoader::Load(path, scene) || scene.meshes.empty()) {
                    std::cerr << "[B_Factory] GLTF animated mesh load failed for " << path << "\n";
                    return nullptr;
                }

                SharedMesh mesh = scene.meshes.front();
                if (!mesh) {
                    std::cerr << "[B_Factory] GLTF animated mesh missing mesh data: " << path << "\n";
                    return nullptr;
                }

                std::shared_ptr<::MeshAnimation> selectedAnimation;
                if (!scene.animations.empty()) {
                    selectedAnimation = scene.animations.front();
                    if (!animPathOrName.empty()) {
                        const std::string animExtension = ExtractExtension(animPathOrName);
                        if (animExtension == ".anm") {
                            auto fallback = std::make_shared<::MeshAnimation>(animPathOrName);
                            if (fallback && fallback->GetJointCount() > 0 && fallback->GetFrameCount() > 0) {
                                selectedAnimation = fallback;
                            }
                            else {
                                std::cerr << "[B_Factory] Fallback animation load failed for "
                                    << animPathOrName << "\n";
                            }
                        }
                        else if (IsDigits(animPathOrName)) {
                            try {
                                const std::size_t index = std::stoul(animPathOrName);
                                if (index < scene.animations.size() && scene.animations[index]) {
                                    selectedAnimation = scene.animations[index];
                                }
                            }
                            catch (const std::exception&) {
                            }
                        }
                    }
                }

                if (!selectedAnimation) {
                    std::cerr << "[B_Factory] GLTF animated mesh missing animation clip: " << path << "\n";
                    return nullptr;
                }

                logAnimatedMesh(path, selectedAnimation);
                return std::make_shared<B_AnimatedMesh>(mesh, selectedAnimation);
            }

            std::shared_ptr<::Mesh> mesh(::Mesh::LoadFromMeshFile(path));
            if (!mesh) {
                std::cerr << "[B_Factory] Animated mesh load failed for " << path << "\n";
                return nullptr;
            }

            std::shared_ptr<::MeshAnimation> animation;
            if (!animPathOrName.empty()) {
                animation = std::make_shared<::MeshAnimation>(animPathOrName);
                if (!animation || animation->GetJointCount() == 0 || animation->GetFrameCount() == 0) {
                    std::cerr << "[B_Factory] Animation clip load failed: " << animPathOrName << "\n";
                    animation.reset();
                }
            }

            if (!animation) {
                std::cerr << "[B_Factory] Animated mesh requires a valid animation clip: " << path << "\n";
                return nullptr;
            }

            logAnimatedMesh(path, animation);
            return std::make_shared<B_AnimatedMesh>(mesh, animation);
        }
        catch (const std::exception& ex) {
            std::cerr << "[B_Factory] Exception while loading animated mesh " << path << ": "
                << ex.what() << "\n";
        }
        return nullptr;
    }

}
