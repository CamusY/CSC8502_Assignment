#pragma once

#include <string>
#include <memory> // 用于 std::shared_ptr
#include <vector> // 用于 LoadCubemap

// NFR-2: 接口直接依赖 nclgl 通用数学库
#include "nclgl/Vector3.h"
// (I_ResourceFactory.h 不需要 Matrix4, Vector2, Vector4)

// IAL 依赖 IAL (纯净)
// (确保这些文件已按前几步的示例创建好)
#include "IAL/I_Shader.h"
#include "IAL/I_Mesh.h"
#include "IAL/I_Texture.h"
#include "IAL/I_Heightmap.h"
#include "IAL/I_AnimatedMesh.h"
#include "IAL/I_FrameBuffer.h"

// NFR-1: 严禁 #include "nclgl/Mesh.h", "nclgl/Shader.h" 等

namespace Engine::IAL {

    /**
     * @brief IAL 资源工厂接口 (核心服务)
     * @details
     * - 这是 Demo 层 (Renderer, Scenes) 与具体实现 (轨道 B/C) 之间的主要桥梁。
     * - NFR-9: Demo 层严禁 new 任何渲染对象，必须通过此工厂接口创建。
     * - NFR-14: 轨道 B (B_Factory) 是核心交付物。
     * - B_Factory 将负责包装 nclgl::Mesh::LoadFromMeshFile, new nclgl::Shader,
     * SOIL_load_OGL_texture, 和自研的 FBO 及 Heightmap 加载器。
     */
    class I_ResourceFactory {
    public:
        virtual ~I_ResourceFactory() {}
        
        // P-0, P-7 (NFR-6: 共享着色器)
        virtual std::shared_ptr<I_Shader> CreateShader(
            const std::string& vPath, 
            const std::string& fPath, 
            const std::string& gPath = "") = 0;
        
        // P-0 (静态模型)
        // NFR-4: 路径由 Demo 层的 Game/Scenes/ 根据宏决定
        // 轨道 B (B_Factory) 仅支持 .msh
        virtual std::shared_ptr<I_Mesh> LoadMesh(const std::string& path) = 0;
        
        // P-0 (纹理)
        virtual std::shared_ptr<I_Texture> LoadTexture(
            const std::string& path, bool repeat = false) = 0;
        
        // P-8 (环境贴图)
        // (B_Factory 将包装 SOIL_load_OGL_cubemap)
        virtual std::shared_ptr<I_Texture> LoadCubemap(
            const std::string& negx, const std::string& posx, 
            const std::string& negy, const std::string& posy, 
            const std::string& negz, const std::string& posz) = 0;
            
        // P-0 (地形)
        // NFR-2: 接口函数签名直接使用 nclgl 数学类型
        virtual std::shared_ptr<I_Heightmap> LoadHeightmap(
            const std::string& path, const Vector3& scale) = 0;
        
        // P-0 (水体), P-3 (后处理)
        // (B_Factory 将包装 nclgl::Mesh::GenerateQuad() (如果存在) 或自研)
        virtual std::shared_ptr<I_Mesh> CreateQuad() = 0;

        // P-4 (阴影)
        // NFR-11.3: B_Factory 必须实例化自研的 B_FrameBuffer
        virtual std::shared_ptr<I_FrameBuffer> CreateShadowFBO(int width, int height) = 0;
        
        // P-3 (后处理)
        // NFR-11.3: B_Factory 必须实例化自研的 B_FrameBuffer
        virtual std::shared_ptr<I_FrameBuffer> CreatePostProcessFBO(int width, int height) = 0;
        
        // P-4 (动画)
        // NFR-4: 路径由 Demo 层的 Game/Scenes/ 根据宏决定
        // B_Factory (轨道 B) 将加载 .msh + .anm
        virtual std::shared_ptr<I_AnimatedMesh> LoadAnimatedMesh(
            const std::string& path, 
            const std::string& animPathOrName = "") = 0;
    };

} // namespace Engine::IAL