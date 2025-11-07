/**
* @file B_Shader.cpp
 * @brief 轨道 B (NCLGL_Impl) 的着色器接口实现源文件。
 *
 * 本文件实现了 B_Shader 类。
 * 在 Day 2 阶段，所有函数均为基础的空壳实现 (Stubs)。
 * 析构函数已包含基础的资源释放逻辑以防止未来开发中的疏忽。
 */
#include "B_Shader.h"
#include "nclgl/Shader.h"

namespace NCLGL_Impl {

    B_Shader::B_Shader(::Shader* shader) : m_shader(shader) {
    }

    B_Shader::~B_Shader() {
        delete m_shader;
    }

    void B_Shader::Bind() {
    }

    void B_Shader::Unbind() {
    }

    void B_Shader::SetUniform(const std::string& name, const Matrix4& mat) {
    }

    void B_Shader::SetUniform(const std::string& name, const Vector3& vec) {
    }

    void B_Shader::SetUniform(const std::string& name, const Vector4& vec) {
    }

    void B_Shader::SetUniform(const std::string& name, float f) {
    }

    void B_Shader::SetUniform(const std::string& name, int i) {
    }

}