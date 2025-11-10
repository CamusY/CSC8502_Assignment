/**
* @file B_Texture.h
 * @brief 轨道 B (NCLGL_Impl) 的纹理接口实现。
 *
 * 本文件定义了 B_Texture 类，它是 Engine::IAL::I_Texture 接口的具体实现。
 * 由于 nclgl 库主要使用原生的 OpenGL 纹理 ID (GLuint)，B_Texture 直接包装了这个 ID。
 *
 * B_Texture 类 (NCLGL_Impl::B_Texture):
 * 继承自 Engine::IAL::I_Texture 纯虚接口。
 * 它是一个轻量级的包装器，用于管理 OpenGL 纹理句柄。
 *
 * 构造函数 B_Texture(unsigned int id, Engine::IAL::TextureType type, unsigned int overrideTarget):
 * 接收一个 OpenGL 纹理 ID (通常由 SOIL 或自研加载代码创建)。
 * 参数 id: 原生的 OpenGL 纹理句柄。
 * 参数 type: 纹理的语义类型（例如 2D、Cubemap、Depth 等）。
 * 参数 overrideTarget: 可选的 OpenGL 绑定目标，若为 0 则根据 type 推导默认值。
 *
 * 析构函数 ~B_Texture():
 * 负责调用 glDeleteTextures 释放纹理资源。
 *
 * 成员函数 GetID():
 * 返回内部包装的 OpenGL 纹理 ID。
 *
 * 成员函数 Bind(int slot):
 * 将此纹理绑定到指定的纹理单元槽位，并根据语义类型决定绑定的 OpenGL 目标。
 *
 * 成员变量 m_id:
 * 类型为 unsigned int (对应 GLuint)，存储 OpenGL 纹理句柄。
 *
 * 成员变量 m_glTarget:
 * 类型为 unsigned int，若非零则表示特定的绑定目标枚举值。
 *
 * 成员变量 m_type:
 * 描述纹理的语义类型，便于调试或自动选择绑定目标。
 */
#pragma once
#include "IAL/I_Texture.h"

#include <glad/glad.h>

namespace NCLGL_Impl {


    class B_Texture : public Engine::IAL::I_Texture {
    public:
        B_Texture(unsigned int id, Engine::IAL::TextureType type, unsigned int overrideTarget = 0);
        ~B_Texture() override;

        unsigned int GetID() override;
        void Bind(int slot = 0) override;
        Engine::IAL::TextureType GetType() const override;

    private:
        unsigned int m_id;
        unsigned int m_glTarget;
        Engine::IAL::TextureType m_type;
    };

}