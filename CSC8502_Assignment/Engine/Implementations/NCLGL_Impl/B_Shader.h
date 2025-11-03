#pragma once

#include "IAL/I_Shader.h" // 包含我们要实现的 IAL 接口
#include <memory>         // 用于 std::unique_ptr
#include <string>
#include <map>

// 1. (规范) 在头文件中使用前向声明
class Shader; 

namespace NCLGL_Impl {

    /**
     * @brief 轨道 B (nclgl) 着色器实现
     * @details
     * - 实现了 Engine::IAL::I_Shader 接口。
     * - 包装 nclgl::Shader 实例。
     * - V13 规范 (4.2)：此类实现了 I_Shader 的核心职责，
     * 即抽象 glUseProgram 和 glUniform... API 调用。
     */
    class B_Shader : public Engine::IAL::I_Shader {
    public:
        /**
         * @brief 构造函数，获取 nclgl::Shader 实例的所有权。
         * @param nclShader B_Factory 通过 new nclgl::Shader 创建的实例指针。
         */
        B_Shader(Shader* nclShader);
        
        /**
         * @brief 析构函数
         * @details std::unique_ptr 将自动删除 m_nclShader
         */
        virtual ~B_Shader();

        // --- I_Shader 接口实现 ---

        virtual void Bind() override;
        virtual void Unbind() override;

        // V13 规范 (NFR-2)：这些函数签名直接使用 nclgl 数学类型
        virtual void SetUniform(const std::string& name, const Matrix4& mat) override;
        virtual void SetUniform(const std::string& name, const Vector3& vec) override;
        virtual void SetUniform(const std::string& name, const Vector4& vec) override;
        virtual void SetUniform(const std::string& name, float f) override;
        virtual void SetUniform(const std::string& name, int i) override;

    private:
        /**
         * @brief 获取并缓存 uniform 变量的位置
         */
        int GetUniformLocation(const std::string& name);

        std::unique_ptr<Shader> m_nclShader; // 2. (规范) 被包装的 nclgl 实例
        unsigned int m_programID;            // 3. (规范) 缓存 nclgl::Shader::GetProgram() 的结果
        
        // 4. (优化) 缓存 uniform location，避免重复查询
        std::map<std::string, int> m_locationCache;
    };

} // namespace NCLGL_Impl