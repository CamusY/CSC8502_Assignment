/**
* @file B_Shader.h
 * @brief 轨道 B (NCLGL_Impl) 的着色器接口实现。
 *
 * 本文件定义了 B_Shader 类，它是 Engine::IAL::I_Shader 接口在 nclgl 框架下的具体适配器实现。
 * 它负责包装原生的 nclgl::Shader 对象，并提供统一的接口用于绑定着色器程序和设置 Uniform 变量。
 *
 * B_Shader 类 (NCLGL_Impl::B_Shader):
 * 继承自 Engine::IAL::I_Shader 纯虚接口。
 * 作为一个适配器，它内部持有一个指向 nclgl::Shader 的原生指针，并接管其生命周期。
 *
 * 构造函数 B_Shader(::Shader* shader):
 * 接收一个 nclgl::Shader 指针。
 * 参数 shader: 指向有效原生着色器对象的指针。
 *
 * 析构函数 ~B_Shader():
 * 负责释放内部持有的 nclgl::Shader 资源。
 *
 * 成员函数 Bind() / Unbind():
 * 实现 I_Shader 定义的绑定和解绑接口。
 * 在完整实现中，将调用 glUseProgram。
 *
 * 成员函数 SetUniform(...):
 * 实现 I_Shader 定义的各种类型的 Uniform 设置接口。
 * 在完整实现中，将调用 glUniform* 系列函数。
 *
 * 成员变量 m_shader:
 * 类型为 ::Shader*，指向被适配的原生 nclgl 着色器对象。
 */
#pragma once
#include "IAL/I_Shader.h"

class Shader;

namespace NCLGL_Impl {

    class B_Shader : public Engine::IAL::I_Shader {
    public:
        explicit B_Shader(::Shader* shader);
        ~B_Shader() override;

        void Bind() override;
        void Unbind() override;

        void SetUniform(const std::string& name, const Matrix4& mat) override;
        void SetUniform(const std::string& name, const Vector3& vec) override;
        void SetUniform(const std::string& name, const Vector4& vec) override;
        void SetUniform(const std::string& name, float f) override;
        void SetUniform(const std::string& name, int i) override;

    private:
        ::Shader* m_shader;
    };

}