#include "B_Shader.h"

// 5. (规范) 包含 nclgl 的具体实现头文件
#include "nclgl/Shader.h" 

// 6. (关键) 包含 glad.h 以便调用原生的 OpenGL API
//    这是 I_Shader 接口抽象的核心
#include "glad/glad.h" 

#include <stdexcept>
#include <iostream> // 用于调试

namespace NCLGL_Impl {

    B_Shader::B_Shader(Shader* nclShader)
        : m_nclShader(nclShader), m_programID(0) {
        if (!m_nclShader) {
            throw std::runtime_error("B_Shader 构造失败：传入的 nclShader 为空指针。");
        }
        
        // 7. (关键) 在构造时缓存 Program ID，这是所有后续 GL 调用的基础
        m_programID = m_nclShader->GetProgram(); //
        
        if (m_programID == 0) {
             std::cerr << "B_Shader 警告：包装的 nclgl::Shader Program ID 为 0。" << std::endl;
        }
    }

    B_Shader::~B_Shader() {
        // m_nclShader 会在这里被 std::unique_ptr 自动 delete
        // nclgl::Shader 的析构函数会调用 glDeleteProgram
    }

    void B_Shader::Bind() {
        // 8. (规范) 抽象 glUseProgram
        glUseProgram(m_programID); //
    }

    void B_Shader::Unbind() {
        glUseProgram(0); //
    }

    int B_Shader::GetUniformLocation(const std::string& name) {
        // 9. (优化) 检查缓存
        auto it = m_locationCache.find(name);
        if (it != m_locationCache.end()) {
            return it->second;
        }

        // 10. (规范) 如果不在缓存中，调用 OpenGL API 查询
        int location = glGetUniformLocation(m_programID, name.c_str()); //
        
        if (location == -1) {
            // (可选的调试信息)
            // std::cerr << "B_Shader 警告：在 Program " << m_programID 
            //           << " 中找不到 Uniform '" << name << "'" << std::endl;
        }

        // 11. (优化) 存入缓存
        m_locationCache[name] = location;
        return location;
    }

    // --- Uniform 实现 ---
    // NFR-2 (数据耦合): 
    // IAL 接口 直接使用 nclgl 数学类型，
    // 因此我们可以安全地访问 .values, .x, .y, .z

    void B_Shader::SetUniform(const std::string& name, const Matrix4& mat) {
        int location = GetUniformLocation(name);
        if (location != -1) {
            //
            glUniformMatrix4fv(location, 1, false, mat.values);
        }
    }

    void B_Shader::SetUniform(const std::string& name, const Vector3& vec) {
        int location = GetUniformLocation(name);
        if (location != -1) {
            //
            glUniform3fv(location, 1, &vec.x); 
        }
    }

    void B_Shader::SetUniform(const std::string& name, const Vector4& vec) {
        int location = GetUniformLocation(name);
        if (location != -1) {
            //
            glUniform4fv(location, 1, &vec.x);
        }
    }

    void B_Shader::SetUniform(const std::string& name, float f) {
        int location = GetUniformLocation(name);
        if (location != -1) {
            //
            glUniform1f(location, f);
        }
    }

    void B_Shader::SetUniform(const std::string& name, int i) {
        int location = GetUniformLocation(name);
        if (location != -1) {
            //
            glUniform1i(location, i);
        }
    }

} // namespace NCLGL_Impl