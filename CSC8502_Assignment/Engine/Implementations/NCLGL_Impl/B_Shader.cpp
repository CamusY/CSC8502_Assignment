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
#include <glad/glad.h>

namespace {
    GLint ResolveLocation(::Shader* shader, const std::string& name) {
        if (!shader) {
            return -1;
        }
        return glGetUniformLocation(shader->GetProgram(), name.c_str());
    }
}

namespace NCLGL_Impl {

    B_Shader::B_Shader(::Shader* shader) : m_shader(shader) {
    }

    B_Shader::~B_Shader() {
        delete m_shader;
    }

    void B_Shader::Bind() {
        if (m_shader) {
            glUseProgram(m_shader->GetProgram());
        }
    }

    void B_Shader::Unbind() {
        glUseProgram(0);
    }

    void B_Shader::SetUniform(const std::string& name, const Matrix4& mat) {
        const GLint location = ResolveLocation(m_shader, name);
        if (location >= 0) {
            glUniformMatrix4fv(location, 1, GL_FALSE, mat.values);
        }
    }

    void B_Shader::SetUniform(const std::string& name, const Vector3& vec) {
        const GLint location = ResolveLocation(m_shader, name);
        if (location >= 0) {
            glUniform3f(location, vec.x, vec.y, vec.z);
        }
    }

    void B_Shader::SetUniform(const std::string& name, const Vector4& vec) {
        const GLint location = ResolveLocation(m_shader, name);
        if (location >= 0) {
            glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
        }
    }

    void B_Shader::SetUniform(const std::string& name, float f) {
        const GLint location = ResolveLocation(m_shader, name);
        if (location >= 0) {
            glUniform1f(location, f);
        }
    }

    void B_Shader::SetUniform(const std::string& name, int i) {
        const GLint location = ResolveLocation(m_shader, name);
        if (location >= 0) {
            glUniform1i(location, i);
        }
    }
    void B_Shader::SetUniformMatrix4Array(const std::string& name, const Matrix4* data, std::size_t count) {
        if (!data || count == 0) {
            return;
        }
        const GLint location = ResolveLocation(m_shader, name);
        if (location >= 0) {
            glUniformMatrix4fv(location, static_cast<GLsizei>(count), GL_FALSE, reinterpret_cast<const GLfloat*>(data));
        }
    }

}