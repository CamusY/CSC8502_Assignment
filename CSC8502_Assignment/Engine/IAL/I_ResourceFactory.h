/**
 * @file I_ResourceFactory.h
 * @brief 定义了资源创建工厂的抽象接口。
 * @details
 * 这是 V13 架构中最重要的服务接口。
 * 它的核心职责是充当 `Demo` 层（应用层） 
 * 与具体实现（轨道 B/C） 之间的
 * **唯一桥梁**，用于创建所有渲染对象（如 Mesh, Shader, FBO 等）。
 *
 * `Application` 和 
 * `Renderer` 
 * 将通过依赖注入（在 `main.cpp` 中）
 * 接收此接口的实例。
 *
 * (NFR-1) 规范约束：本文件严禁包含任何 nclgl 的非数学系统级头文件
 * (如 nclgl/Mesh.h, 
 * nclgl/Shader.h 等)。
 *
 * (NFR-2) 规范要求：本文件被授权且必须包含 nclgl 数学库（如 Vector3.h）
 * 以便在函数签名中直接使用 nclgl 的具体数学类型。
 *
 * @see main.cpp (NFR-9 依赖注入点)
 * @see I_Mesh, I_Shader, I_Texture, I_Heightmap, I_AnimatedMesh, I_FrameBuffer
 *
 * @class Engine::IAL::I_ResourceFactory
 * @brief 资源创建工厂的纯虚接口。
 * @details
 * 所有方法均返回 IAL 接口的共享指针（`std::shared_ptr`），
 * 以便在 `SceneGraph` 
 * 和 `Renderer` 
 * 中管理资源生命周期。
 *
 * @fn Engine::IAL::I_ResourceFactory::~I_ResourceFactory
 * @brief 虚拟析构函数。
 *
 * @fn Engine::IAL::I_ResourceFactory::CreateShader
 * @brief (P-0, P-7) 
 * 从 GLSL 文件创建（编译和链接）一个着色器程序。
 * @details (NFR-6) 
 * 路径应指向 `Assets/Shaders/Shared/` 目录。
 * 轨道 B 实现将包装 `new nclgl::Shader(...)`。
 * @param vPath 顶点着色器文件路径。
 * @param fPath 片段着色器文件路径。
 * @param gPath (可选) 几何着色器文件路径。
 * @return `std::shared_ptr<I_Shader>` 接口。
 *
 * @fn Engine::IAL::I_ResourceFactory::LoadMesh
 * @brief (P-0, P-2) 
 * 从文件加载一个静态网格。
 * @details (NFR-3) 
 * 轨道 B 实现 `B_Factory` 必须使用
 * `nclgl/Extra/GLTFLoader.h` 
 * 来支持 `.gltf` 格式。
 * @param path 模型文件路径（例如 "Assets/Models/ruins.gltf"）。
 * @return `std::shared_ptr<I_Mesh>` 接口。
 *
 * @fn Engine::IAL::I_ResourceFactory::LoadTexture
 * @brief (P-0) 
 * 从文件加载一个 2D 纹理。
 * @details 轨道 B 实现将包装 `SOIL_load_OGL_texture`。
 * @param path 纹理文件路径（例如 .png, .jpg）。
 * @param repeat 是否设置纹理环绕模式为 `GL_REPEAT`（默认为 `GL_CLAMP`）。
 * @return `std::shared_ptr<I_Texture>` 接口。
 *
 * @fn Engine::IAL::I_ResourceFactory::LoadCubemap
 * @brief (P-8) 
 * 从六个单独的文件加载一个立方体贴图（用于天空盒）。
 * @details 轨道 B 实现将包装 `SOIL_load_OGL_cubemap`。
 * @param negx (Left)
 * @param posx (Right)
 * @param negy (Bottom)
 * @param posy (Top)
 * @param negz (Back)
 * @param posz (Front)
 * @return `std::shared_ptr<I_Texture>` 接口。
 *
 * @fn Engine::IAL::I_ResourceFactory::LoadHeightmap
 * @brief (P-0) 
 * 从 `.raw` 高度图文件加载一个地形网格。
 * @details (NFR-11.3) 
 * `nclgl` 
 * 缺乏此功能。轨道 B 实现 `B_Factory`
 * 必须自研 `.raw` 加载器，并在内部使用继承了 `nclgl::Mesh`
 * 的辅助类来填充顶点数据。
 * (NFR-2) 
 * `scale` 参数必须使用 `nclgl::Vector3`。
 * @param path `.raw` 文件路径。
 * @param scale `nclgl::Vector3` 类型的缩放因子（x, y, z）。
 * @return `std::shared_ptr<I_Heightmap>` 接口。
 *
 * @fn Engine::IAL::I_ResourceFactory::CreateQuad
 * @brief (P-0, P-3) 
 * 创建一个覆盖全屏的 NDC 坐标四边形网格。
 * @details (NFR-11.3) 
 * `nclgl::Mesh` 
 * 缺乏 `GenerateQuad`。
 * 轨道 B 实现 `B_Factory` 必须采用与 `LoadHeightmap` 相同的
 * “自研+继承” 策略来创建此网格。
 * 用于水体 (P-0) 和后处理 (P-3)。
 * @return `std::shared_ptr<I_Mesh>` 接口。
 *
 * @fn Engine::IAL::I_ResourceFactory::CreateShadowFBO
 * @brief (P-4) 
 * 创建一个用于阴影贴图的帧缓冲对象（FBO）。
 * @details (NFR-11.3) 
 * 轨道 B 实现将实例化自研的 `B_FrameBuffer`。
 * 此 FBO 通常只包含一个深度纹理附件。
 * @param width FBO 宽度（阴影贴图分辨率）。
 * @param height FBO 高度（阴影贴图分辨率）。
 * @return `std::shared_ptr<I_FrameBuffer>` 接口。
 *
 * @fn Engine::IAL::I_ResourceFactory::CreatePostProcessFBO
 * @brief (P-3) 
 * 创建一个用于后期处理的帧缓冲对象（FBO）。
 * @details (NFR-11.3) 
 * 轨道 B 实现将实例化自研的 `B_FrameBuffer`。
 * 此 FBO 通常包含颜色和深度纹理附件。
 * @param width FBO 宽度（通常为屏幕分辨率）。
 * @param height FBO 高度（通常为屏幕分辨率）。
 * @return `std::shared_ptr<I_FrameBuffer>` 接口。
 *
 * @fn Engine::IAL::I_ResourceFactory::LoadAnimatedMesh
 * @brief (P-4) 
 * 从文件加载一个带骨骼动画的网格。
 * @details (NFR-3, NFR-11.2)
 * 轨道 B 实现 `B_Factory` 必须使用
 * `nclgl/Extra/GLTFLoader.h`
 * 来加载 `.gltf` 文件，
 * 并从 `GLTFScene` 
 * 对象中提取 `nclgl::Mesh`
 * 和 `nclgl::MeshAnimation` 
 * 指针，
 * 将它们包装在 `B_AnimatedMesh` 
 * 适配器中返回。
 * @param path 模型文件路径（例如 "Assets/Models/character.gltf"）。
 * @param animPathOrName (可选) 动画路径或名称，主要用于
 * 轨道 B 旧版 .msh/.anm 
 * 格式，或在 .gltf 中选择特定动画。
 * @return `std::shared_ptr<I_AnimatedMesh>` 接口。
 */

#pragma once

#include <string>
#include <memory>

#include "nclgl/Vector3.h"

#include "IAL/I_Shader.h"
#include "IAL/I_Mesh.h"
#include "IAL/I_Texture.h"
#include "IAL/I_Heightmap.h"
#include "IAL/I_AnimatedMesh.h"
#include "IAL/I_FrameBuffer.h"

namespace Engine::IAL {
    class I_ResourceFactory {
    public:
        virtual ~I_ResourceFactory() {}

        virtual std::shared_ptr<I_Shader> CreateShader(
            const std::string& vPath,
            const std::string& fPath,
            const std::string& gPath = "") = 0;

        virtual std::shared_ptr<I_Mesh> LoadMesh(const std::string& path) = 0;

        virtual std::shared_ptr<I_Texture> LoadTexture(
            const std::string& path, bool repeat = false) = 0;

        virtual std::shared_ptr<I_Texture> LoadCubemap(
            const std::string& negx, const std::string& posx,
            const std::string& negy, const std::string& posy,
            const std::string& negz, const std::string& posz) = 0;

        virtual std::shared_ptr<I_Heightmap> LoadHeightmap(
            const std::string& path, const Vector3& scale) = 0;

        virtual std::shared_ptr<I_Mesh> CreateQuad() = 0;

        virtual std::shared_ptr<I_FrameBuffer> CreateShadowFBO(int width, int height) = 0;

        virtual std::shared_ptr<I_FrameBuffer> CreatePostProcessFBO(int width, int height) = 0;

        virtual std::shared_ptr<I_AnimatedMesh> LoadAnimatedMesh(
            const std::string& path,
            const std::string& animPathOrName = "") = 0;
    };

}
