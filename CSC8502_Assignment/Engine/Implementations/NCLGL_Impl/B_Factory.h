#pragma once

#include "IAL/I_ResourceFactory.h" // 1. (规范) 包含我们要实现的 IAL 接口
#include <map>
#include <string>

// 2. (规范) NFR-1
//    B_Factory.h 自身也应该保持纯净，不暴露 nclgl 实现细节。
//    我们只包含 IAL 接口和 nclgl 数学库。
#include "nclgl/Vector3.h"

// (前向声明 IAL 接口，虽然 I_ResourceFactory.h 已经包含了它们)
namespace Engine::IAL {
    class I_Shader;
    class I_Mesh;
    class I_Texture;
    class I_Heightmap;
    class I_AnimatedMesh;
    class I_FrameBuffer;
}

namespace NCLGL_Impl {

    /**
     * @brief 轨道 B (nclgl) 资源工厂实现
     * @details
     * - 实现了 Engine::IAL::I_ResourceFactory 接口。
     * - NFR-9: 
     * 这是轨道 B 的核心服务，负责创建所有渲染对象的适配器实例。
     * - NFR-1: 
     * B_Factory.cpp 是唯一被授权 #include "nclgl/Mesh.h", "nclgl/Shader.h", "SOIL/SOIL.h" 
     * 等具体实现的地方。
     * - NFR-11.3: 
     * 此类必须包含 FBO、
     * Heightmap 和 
     * Quad 的自研实现逻辑。
     */
    class B_Factory : public Engine::IAL::I_ResourceFactory {
    public:
        B_Factory();
        virtual ~B_Factory();

        // --- I_ResourceFactory 接口实现 ---

        virtual std::shared_ptr<Engine::IAL::I_Shader> CreateShader(
            const std::string& vPath, 
            const std::string& fPath, 
            const std::string& gPath = "") override;
        
        virtual std::shared_ptr<Engine::IAL::I_Mesh> LoadMesh(const std::string& path) override;
        
        virtual std::shared_ptr<Engine::IAL::I_Texture> LoadTexture(
            const std::string& path, bool repeat = false) override;
        
        virtual std::shared_ptr<Engine::IAL::I_Texture> LoadCubemap(
            const std::string& negx, const std::string& posx, 
            const std::string& negy, const std::string& posy, 
            const std::string& negz, const std::string& posz) override;
            
        virtual std::shared_ptr<Engine::IAL::I_Heightmap> LoadHeightmap(
            const std::string& path, const Vector3& scale) override;
        
        virtual std::shared_ptr<Engine::IAL::I_Mesh> CreateQuad() override;

        virtual std::shared_ptr<Engine::IAL::I_FrameBuffer> CreateShadowFBO(int width, int height) override;
        
        virtual std::shared_ptr<Engine::IAL::I_FrameBuffer> CreatePostProcessFBO(int width, int height) override;
        
        virtual std::shared_ptr<Engine::IAL::I_AnimatedMesh> LoadAnimatedMesh(
            const std::string& path, 
            const std::string& animPathOrName = "") override;

    private:
        // (可选) 缓存已加载的资源，防止重复加载
        std::map<std::string, std::shared_ptr<Engine::IAL::I_Mesh>> m_meshCache;
        std::map<std::string, std::shared_ptr<Engine::IAL::I_Texture>> m_textureCache;
        std::map<std::string, std::shared_ptr<Engine::IAL::I_Shader>> m_shaderCache;
        
        // 缓存生成的 Quad
        std::shared_ptr<Engine::IAL::I_Mesh> m_quadMesh;
    };

} // namespace NCLGL_Impl