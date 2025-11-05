/**
 * @file I_Shader.h
 * @brief 定义了 GPU 着色器程序（Shader Program）的抽象接口。
 * @details
 * 该文件的设计目的是为核心渲染器（Renderer）提供一个统一的、与后端无关的方式来
 * 绑定/解绑着色器程序，以及设置其 Uniform 变量（如 P-7 光照 
 * 或 P-4 骨骼 矩阵）。
 *
 * (NFR-1) 规范约束：本文件严禁包含 nclgl/Shader.h。
 * (NFR-2) 规范要求：本文件被授权且必须包含 nclgl 数学库（如 Matrix4.h,
 * Vector3.h 等），以便在 `SetUniform` 函数签名中
 * 直接使用 nclgl 的具体数学类型。
 *
 * @see I_ResourceFactory::CreateShader
 *
 * @class Engine::IAL::I_Shader
 * @brief 着色器程序的纯虚接口。
 * @details
 * 抽象了 nclgl::Shader 或自研 C_Shader 的功能。
 * 实例由 I_ResourceFactory::CreateShader() 创建。
 *
 * @fn Engine::IAL::I_Shader::~I_Shader
 * @brief 虚拟析构函数。
 *
 * @fn Engine::IAL::I_Shader::Bind
 * @brief 绑定此着色器程序以供后续渲染操作使用。
 * @details
 * 抽象了 `glUseProgram(programID)`。
 * 在 `nclgl` 轨道 B 中，将调用 `nclgl::Shader::Bind()`。
 *
 * @fn Engine::IAL::I_Shader::Unbind
 * @brief 解绑当前着色器程序。
 * @details
 * 抽象了 `glUseProgram(0)`。
 * 在 `nclgl` 轨道 B 中，将调用 `nclgl::Shader::Unbind()`。
 *
 * @fn Engine::IAL::I_Shader::SetUniform(const std::string& name, const Matrix4& mat)
 * @brief (NFR-2) 
 * 设置一个 mat4 类型的 Uniform 变量，使用 nclgl::Matrix4。
 * @details 抽象了 `glUniformMatrix4fv`。
 * @param name Uniform 变量在 GLSL 中的名称。
 * @param mat 要设置的 nclgl::Matrix4 矩阵。
 *
 * @fn Engine::IAL::I_Shader::SetUniform(const std::string& name, const Vector3& vec)
 * @brief (NFR-2) 
 * 设置一个 vec3 类型的 Uniform 变量，使用 nclgl::Vector3。
 * @details 抽象了 `glUniform3fv`。
 * @param name Uniform 变量在 GLSL 中的名称。
 * @param vec 要设置的 nclgl::Vector3 向量。
 *
 * @fn Engine::IAL::I_Shader::SetUniform(const std::string& name, const Vector4& vec)
 * @brief (NFR-2) 
 * 设置一个 vec4 类型的 Uniform 变量，使用 nclgl::Vector4。
 * @details 抽象了 `glUniform4fv`。
 * @param name Uniform 变量在 GLSL 中的名称。
 * @param vec 要设置的 nclgl::Vector4 向量。
 *
 * @fn Engine::IAL::I_Shader::SetUniform(const std::string& name, float f)
 * @brief (NFR-2) 
 * 设置一个 float 类型的 Uniform 变量。
 * @details 抽象了 `glUniform1f`。
 * @param name Uniform 变量在 GLSL 中的名称。
 * @param f 要设置的 float 浮点数。
 *
 * @fn Engine::IAL::I_Shader::SetUniform(const std::string& name, int i)
 * @brief (NFR-2) 
 * 设置一个 int 类型的 Uniform 变量（常用于设置纹理采样器单元）。
 * @details 抽象了 `glUniform1i`。
 * @param name Uniform 变量在 GLSL 中的名称。
 * @param i 要设置的 int 整数。
 */

#pragma once

#include <string>

// (NFR-2) 接口直接依赖 nclgl 通用数学库
#include "nclgl/Matrix4.h"
#include "nclgl/Vector3.h"
#include "nclgl/Vector4.h"

namespace Engine::IAL
{
    class I_Shader
    {
    public:
        virtual ~I_Shader() {}
        
        virtual void Bind() = 0;   
        virtual void Unbind() = 0;
        
        virtual void SetUniform(const std::string& name, const Matrix4& mat) = 0;
        virtual void SetUniform(const std::string& name, const Vector3& vec) = 0;
        virtual void SetUniform(const std::string& name, const Vector4& vec) = 0;
        virtual void SetUniform(const std::string& name, float f) = 0;
        virtual void SetUniform(const std::string& name, int i) = 0;
    };
    
}