#include "B_Factory.h"

// 3. (规范) NFR-1: 
//    包含所有 nclgl 库的 "具体实现" 和我们自己的 "轨道 B 适配器"
#include "nclgl/Shader.h"           //
#include "nclgl/Mesh.h"             //
#include "nclgl/MeshAnimation.h"    //
#include "nclgl/OGLRenderer.h"      // 
                                    // (为了 SOIL.h 和 glad.h)
#include "nclgl/Vector2.h"          //
#include "nclgl/Vector3.h"          //
#include "nclgl/Vector4.h"          //

#include "B_Shader.h"
#include "B_Mesh.h"
#include "B_Texture.h"
#include "B_Heightmap.h"
#include "B_AnimatedMesh.h"
#include "B_FrameBuffer.h"

#include <stdexcept>
#include <iostream>
#include <fstream>

namespace NCLGL_Impl {

    // 4. (关键解决方案) 
    //    定义一个匿名的命名空间，用于存放我们的辅助类
    //    这些类继承 nclgl::Mesh 以访问 protected 成员
    namespace {
        /**
         * @brief NFR-11.3 
         * 辅助类：用于生成全屏 Quad
         * @details
         * - 继承 nclgl::Mesh 
         * 以访问 protected 成员
         * - 在构造函数中填充顶点数据并调用 BufferData()
         */
        class B_QuadMesh_NCLGL : public Mesh {
        public:
            B_QuadMesh_NCLGL() {
                // 访问 protected 成员
                numVertices = 4;
                type = GL_TRIANGLE_STRIP;
                vertices = new Vector3[numVertices];
                textureCoords = new Vector2[numVertices];

                vertices[0] = Vector3(-1, -1, 0);
                vertices[1] = Vector3(1, -1, 0);
                vertices[2] = Vector3(-1, 1, 0);
                vertices[3] = Vector3(1, 1, 0);
                
                textureCoords[0] = Vector2(0, 0);
                textureCoords[1] = Vector2(1, 0);
                textureCoords[2] = Vector2(0, 1);
                textureCoords[3] = Vector2(1, 1);
                
                // 调用 protected 方法
                BufferData(); 
            }
        };

        /**
         * @brief NFR-11.3 
         * 辅助类：用于加载 .raw 高度图
         * @details
         * - 继承 nclgl::Mesh
         * - 构造函数负责加载 .raw 文件、填充 protected 成员并调用 BufferData()
         */
        class B_HeightmapMesh_NCLGL : public Mesh {
        public:
            B_HeightmapMesh_NCLGL(const std::string& path, const Vector3& scale) {
                std::ifstream file(path.c_str(), std::ios::binary);
                if (!file) {
                    throw std::runtime_error("B_Factory: 无法打开 .raw 高度图文件: " + path);
                }

                file.seekg(0, std::ios::end);
                int length = (int)file.tellg();
                file.seekg(0, std::ios::beg);
                
                int width = (int)sqrt(length);
                int height = width;

                if (width * height != length) {
                    file.close();
                    throw std::runtime_error("B_Factory: 高度图 .raw 文件不是正方形: " + path);
                }

                unsigned char* data = new unsigned char[length];
                file.read((char*)data, length);
                file.close();

                // 访问 protected 成员
                numVertices = width * height;
                numIndices = (width - 1) * (height - 1) * 6;
                type = GL_TRIANGLES;
                vertices = new Vector3[numVertices];
                textureCoords = new Vector2[numVertices];
                indices = new unsigned int[numIndices];

                for (int y = 0; y < height; ++y) {
                    for (int x = 0; x < width; ++x) {
                        int index = (y * width) + x;
                        float vertexHeight = (float)data[index] / 255.0f;
                        
                        vertices[index] = Vector3(
                            (float)x * scale.x,
                            vertexHeight * scale.y,
                            (float)y * scale.z
                        );
                        
                        textureCoords[index] = Vector2(
                            (float)x / (width - 1),
                            (float)y / (height - 1)
                        );
                    }
                }
                delete[] data;

                int i = 0;
                for (int y = 0; y < height - 1; ++y) {
                    for (int x = 0; x < width - 1; ++x) {
                        int a = (y * width) + x;
                        int b = (y * width) + (x + 1);
                        int c = ((y + 1) * width) + (x + 1);
                        int d = ((y + 1) * width) + x;

                        indices[i++] = a;
                        indices[i++] = d;
                        indices[i++] = c;
                        indices[i++] = c;
                        indices[i++] = b;
                        indices[i++] = a;
                    }
                }

                // 调用 protected 方法
                BufferData(); 
            }
        };
    } // 结束 匿名命名空间


    // --- B_Factory 实现 ---

    B_Factory::B_Factory() {
        // (可选) 可以在此预加载资源
    }

    B_Factory::~B_Factory() {
        // 缓存的 shared_ptr 会在这里自动销毁
    }

    // --- Shader ---
    std::shared_ptr<Engine::IAL::I_Shader> B_Factory::CreateShader(
        const std::string& vPath, const std::string& fPath, const std::string& gPath) {
        
        std::string cacheKey = vPath + fPath + gPath;
        if (m_shaderCache.count(cacheKey)) {
            return m_shaderCache[cacheKey];
        }

        // 5. (规范) B_Factory 调用 nclgl 构造函数
        Shader* nclShader = new Shader(vPath, fPath, gPath);
        if (!nclShader->LoadSuccess()) {
            delete nclShader;
            throw std::runtime_error("B_Factory: nclgl::Shader 编译失败: " + vPath);
        }

        // 6. (规范) 将 nclgl 实例包装在 B_Shader 适配器中
        auto ialShader = std::make_shared<B_Shader>(nclShader);
        m_shaderCache[cacheKey] = ialShader;
        return ialShader;
    }

    // --- Mesh ---
    std::shared_ptr<Engine::IAL::I_Mesh> B_Factory::LoadMesh(const std::string& path) {
        if (m_meshCache.count(path)) {
            return m_meshCache[path];
        }

        // 7. (规范) B_Factory 调用 nclgl 加载函数
        Mesh* nclMesh = Mesh::LoadFromMeshFile(path); // 仅支持 .msh
        if (!nclMesh) {
            throw std::runtime_error("B_Factory: nclgl::Mesh::LoadFromMeshFile 失败: " + path);
        }

        auto ialMesh = std::make_shared<B_Mesh>(nclMesh);
        m_meshCache[path] = ialMesh;
        return ialMesh;
    }

    // --- Texture ---
    std::shared_ptr<Engine::IAL::I_Texture> B_Factory::LoadTexture(
        const std::string& path, bool repeat) {
        
        if (m_textureCache.count(path)) {
            return m_textureCache[path];
        }

        // 8. (规范) B_Factory 直接调用 SOIL
        GLuint textureID = SOIL_load_OGL_texture(
            path.c_str(),
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y // nclgl 默认使用 INVERT_Y
        );

        if (textureID == 0) {
            throw std::runtime_error("B_Factory: SOIL_load_OGL_texture 失败: " + path);
        }

        // 9. (规范) 设置纹理参数 (nclgl 默认行为)
        glBindTexture(GL_TEXTURE_2D, textureID); //
        if (repeat) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); //
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //
        }
        glBindTexture(GL_TEXTURE_2D, 0); //

        auto ialTexture = std::make_shared<B_Texture>(textureID, GL_TEXTURE_2D);
        m_textureCache[path] = ialTexture;
        return ialTexture;
    }

    // --- Cubemap ---
    std::shared_ptr<Engine::IAL::I_Texture> B_Factory::LoadCubemap(
        const std::string& negx, const std::string& posx, 
        const std::string& negy, const std::string& posy, 
        const std::string& negz, const std::string& posz) {

        std::string cacheKey = negx + posx; // 简单缓存键
        if (m_textureCache.count(cacheKey)) {
            return m_textureCache[cacheKey];
        }

        // 10. (规范) B_Factory 调用 SOIL 的 cubemap 加载器
        GLuint textureID = SOIL_load_OGL_cubemap(
            posx.c_str(), negx.c_str(),
            posy.c_str(), negy.c_str(),
            posz.c_str(), negz.c_str(),
            SOIL_LOAD_RGB,
            SOIL_CREATE_NEW_ID,
            0 // nclgl 示例中通常不为 Cubemap 生成 Mipmap
        );

        if (textureID == 0) {
            throw std::runtime_error("B_Factory: SOIL_load_OGL_cubemap 失败: " + posx);
        }

        auto ialTexture = std::make_shared<B_Texture>(textureID, GL_TEXTURE_CUBE_MAP);
        m_textureCache[cacheKey] = ialTexture;
        return ialTexture;
    }

    // --- Heightmap ---
    std::shared_ptr<Engine::IAL::I_Heightmap> B_Factory::LoadHeightmap(
        const std::string& path, const Vector3& scale) {
        
        // 11. (解决方案)
        //     实例化我们继承自 nclgl::Mesh 
        //     的辅助类 B_HeightmapMesh_NCLGL
        Mesh* nclMesh = new B_HeightmapMesh_NCLGL(path, scale);
        
        // 12. (规范) 将这个 nclMesh 实例包装在 B_Heightmap 适配器中
        return std::make_shared<B_Heightmap>(nclMesh);
    }

    // --- Quad ---
    std::shared_ptr<Engine::IAL::I_Mesh> B_Factory::CreateQuad() {
        if (m_quadMesh) {
            return m_quadMesh;
        }

        // 13. (解决方案)
        //     实例化我们继承自 nclgl::Mesh 
        //     的辅助类 B_QuadMesh_NCLGL
        Mesh* nclMesh = new B_QuadMesh_NCLGL();
        
        m_quadMesh = std::make_shared<B_Mesh>(nclMesh);
        return m_quadMesh;
    }

    // --- FBOs ---
    std::shared_ptr<Engine::IAL::I_FrameBuffer> B_Factory::CreateShadowFBO(int width, int height) {
        // 14. (规范) NFR-11.3
        //     实例化我们自研的 B_FrameBuffer (P-4 阴影)
        //     阴影 FBO 只需要深度附件
        return std::make_shared<B_FrameBuffer>(width, height, false, true);
    }

    std::shared_ptr<Engine::IAL::I_FrameBuffer> B_Factory::CreatePostProcessFBO(int width, int height) {
        // 15. (规范) NFR-11.3
        //     实例化 B_FrameBuffer (P-3 后处理)
        //     后处理 FBO 至少需要颜色附件（通常也需要深度）
        return std::make_shared<B_FrameBuffer>(width, height, true, true);
    }

    // --- Animated Mesh ---
    std::shared_ptr<Engine::IAL::I_AnimatedMesh> B_Factory::LoadAnimatedMesh(
        const std::string& path, const std::string& animPathOrName) {

        // 16. (规范) P-4
        //     轨道 B 加载 .msh (模型) 
        //     和 .anm (动画)
        
        Mesh* nclMesh = Mesh::LoadFromMeshFile(path);
        if (!nclMesh) {
            throw std::runtime_error("B_Factory: LoadAnimatedMesh (Mesh) 失败: " + path);
        }

        MeshAnimation* nclAnimation = new MeshAnimation(animPathOrName);
        if (nclAnimation->GetFrameCount() == 0) { //
            delete nclMesh;
            delete nclAnimation;
            throw std::runtime_error("B_Factory: LoadAnimatedMesh (Animation) 失败: " + animPathOrName);
        }

        // 17. (规范) NFR-11.2
        //      将两者包装在 B_AnimatedMesh 适配器中
        return std::make_shared<B_AnimatedMesh>(nclMesh, nclAnimation);
    }

} // namespace NCLGL_Impl