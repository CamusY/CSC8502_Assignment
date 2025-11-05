/**
* @file I_Texture.h
 * @brief 定义了 GPU 纹理对象（Texture）的抽象接口。
 * @details
 * 该文件的设计目的是为核心引擎提供一个统一的、与后端无关的方式来操作纹理。
 *
 * (NFR-1) 规范约束：本文件严禁包含任何 nclgl 的非数学系统级头文件。
 *
 * 架构说明（V13）：
 * nclgl 库本身没有一个统一的 C++ 纹理类（它主要依赖 SOIL 库直接返回 GLuint）。
 * 因此，`B_Texture`（轨道 B 实现） 
 * 将是一个轻量级包装器，主要用于存储和管理
 * 由 `SOIL_load_OGL_texture` 或 `SOIL_load_OGL_cubemap` 
 * 返回的 `unsigned int` (GLuint) 句柄。
 * 此接口也用于 `I_FrameBuffer` 返回的颜色和深度附件。
 *
 * @see I_ResourceFactory::LoadTexture
 * @see I_ResourceFactory::LoadCubemap
 * @see I_FrameBuffer::GetColorTexture
 *
 * @class Engine::IAL::I_Texture
 * @brief 纹理对象的纯虚接口。
 *
 * @fn Engine::IAL::I_Texture::~I_Texture
 * @brief 虚拟析构函数。
 *
 * @fn Engine::IAL::I_Texture::GetID
 * @brief 获取此纹理的底层图形 API 句柄（例如 OpenGL 中的 GLuint）。
 * @details
 * 渲染器需要此 ID 以便将其传递给 `glActiveTexture` 和 `glBindTexture`。
 * @return unsigned int 类型的纹理 ID。
 *
 * @fn Engine::IAL::I_Texture::Bind(int slot)
 * @brief 将此纹理绑定到指定的纹理槽（Texture Unit Slot）。
 * @details
 * 抽象了 `glActiveTexture(GL_TEXTURE0 + slot)` 和
 * `glBindTexture(GL_TEXTURE_2D, GetID())`（或 GL_TEXTURE_CUBE_MAP）。
 * @param slot 要绑定的纹理单元索引，默认为 0。
 */

#pragma once

namespace Engine::IAL
{
    class I_Texture
    {
    public:
        virtual ~I_Texture() {}
        virtual unsigned int GetID() = 0;
        virtual void Bind(int slot = 0) = 0;
    };
    
}