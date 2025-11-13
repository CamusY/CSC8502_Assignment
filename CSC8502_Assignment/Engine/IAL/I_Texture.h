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
 * 类型语义：
 * 区分纹理的语义类别有助于渲染调度、日志诊断和绑定策略。
 * 例如，颜色附件、深度附件或 Cubemap 在默认绑定目标和用途上存在差异。
 * 通过 `TextureType` 枚举，调用端可以在不直接依赖底层图形 API 枚举的情况下
 * 了解纹理的意图，从而在 `Bind` 或调试输出时选择恰当的行为。
 *
 * @see I_ResourceFactory::LoadTexture
 * @see I_ResourceFactory::LoadCubemap
 * @see I_FrameBuffer::GetColorTexture
 * 
 * @enum TextureType
 * @brief 描述纹理对象的语义类别。
 * @details
 * 渲染器或资源工厂可以依据此枚举决定默认的图形 API 绑定目标、
 * 采样策略或调试输出内容。
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
 * `glBindTexture(target, GetID())`（其中 target 通常依据 `TextureType`
 * 决定，例如 `GL_TEXTURE_2D`、`GL_TEXTURE_CUBE_MAP` 等）。
 * @param slot 要绑定的纹理单元索引，默认为 0。
 *
 * @fn Engine::IAL::I_Texture::GetType
 * @brief 获取纹理的语义类型。
 * @details
 * 渲染后端可以根据类型推导默认的绑定目标或输出更加明确的调试信息，
 * 例如区分颜色附件、深度附件或 Cubemap 资源。
 * @return TextureType 枚举值，表示纹理的语义类别。
 */

#pragma once

namespace Engine::IAL {
    enum class TextureType {
        Texture2D,
        CubeMap,
        Array2D,
        DepthStencil,
        External
    };

    class I_Texture {
    public:
        virtual ~I_Texture() {}
        virtual unsigned int GetID() = 0;
        virtual void Bind(int slot = 0) = 0;
        virtual TextureType GetType() const = 0;
    };

}