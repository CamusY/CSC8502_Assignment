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
* 构造函数 B_Texture(unsigned int id, unsigned int target):
* 接收一个 OpenGL 纹理 ID (通常由 SOIL 或自研加载代码创建)。
* 参数 id: 原生的 OpenGL 纹理句柄。
* 参数 target: 纹理绑定目标（例如 GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP 等）。
*
* 析构函数 ~B_Texture():
* 负责调用 glDeleteTextures 释放纹理资源。
*
* 成员函数 GetID():
* 返回内部包装的 OpenGL 纹理 ID。
*
* 成员函数 Bind(int slot):
* 将此纹理绑定到指定的纹理单元槽位，并绑定到对应的 OpenGL 目标。
*
* 成员变量 m_id:
* 类型为 unsigned int (对应 GLuint)，存储 OpenGL 纹理句柄。
*
* 成员变量 m_target:
* 类型为 unsigned int，存储 glBindTexture 所需的目标枚举值。
*/
#pragma once
#include "IAL/I_Texture.h"

#include <glad/glad.h>

namespace NCLGL_Impl {

    class B_Texture : public Engine::IAL::I_Texture {
    public:
        B_Texture(unsigned int id, unsigned int target);
        ~B_Texture() override;

        unsigned int GetID() override;
        void Bind(int slot = 0) override;

    private:
        unsigned int m_id;
        unsigned int m_target;
    };

}