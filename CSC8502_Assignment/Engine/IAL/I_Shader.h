#pragma once

#include <string>

// NFR-2: 接口直接依赖 nclgl 通用数学库
#include "nclgl/Matrix4.h"
#include "nclgl/Vector3.h"
#include "nclgl/Vector4.h"
// (I_Shader.h 不需要 Vector2)

// NFR-1: 严禁 #include "nclgl/Shader.h"

namespace Engine::IAL {

    /**
     * @brief IAL 着色器程序接口 (P-0, P-7)
     * @details
     * - 抽象了 nclgl::Shader 的核心功能，
     * 以及 nclgl::OGLRenderer::UpdateShaderMatrices 中的 glUniform 功能。
     * - Demo 层的 Renderer 将依赖此接口来绑定着色器和设置 uniforms。
     * - Bind: 抽象 glUseProgram(programID)。
     * - Unbind: 抽象 glUseProgram(0)。
     * - SetUniform: 抽象 glGetUniformLocation + glUniformMatrix4fv 等。
     * B_Shader (轨道 B) 的实现将需要持有 nclgl::Shader* 并调用其 GetProgram()
     * 以获取 programID 来执行 glUniform。
     * - NFR-2: 所有 SetUniform 函数签名必须直接使用 nclgl 数学类型。
     */
    class I_Shader {
    public:
        virtual ~I_Shader() {}
        
        virtual void Bind() = 0;   
        virtual void Unbind() = 0; 
        
        virtual void SetUniform(const std::string& name, const Matrix4& mat) = 0;
        virtual void SetUniform(const std::string& name, const Vector3& vec) = 0;
        virtual void SetUniform(const std::string& name, const Vector4& vec) = 0;
        virtual void SetUniform(const std::string& name, float f) = 0;
        virtual void SetUniform(const std::string& name, int i) = 0;
        
        // (可选扩展: 根据需要添加对 Vector2, bool, 或 uniform 数组的支持)
        // virtual void SetUniform(const std::string& name, const Vector2& vec) = 0;
        // virtual void SetUniform(const std::string& name, bool b) = 0;
    };
    
} // namespace Engine::IAL