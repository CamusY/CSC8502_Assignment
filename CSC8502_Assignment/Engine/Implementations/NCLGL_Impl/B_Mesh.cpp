#include "B_Mesh.h"

// 3. (规范) 在 .cpp 文件中包含 nclgl 的具体实现头文件
#include "nclgl/Mesh.h"
#include <stdexcept> // 用于异常处理

namespace NCLGL_Impl {

    B_Mesh::B_Mesh(Mesh* nclMesh) 
        : m_nclMesh(nclMesh) { // 4. (规范) m_nclMesh (unique_ptr) 获得 nclMesh 的所有权
        if (!m_nclMesh) {
            throw std::runtime_error("B_Mesh construction failed: the provided nclMesh pointer is null.");
        }
    }

    B_Mesh::~B_Mesh() {
        // m_nclMesh 会在这里被 std::unique_ptr 自动 delete
    }

    void B_Mesh::Draw() {
        // 5. (规范) 将 IAL 调用转发给 nclgl 的具体实现
        m_nclMesh->Draw(); //
    }

} // namespace NCLGL_Impl