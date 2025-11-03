#include "B_Heightmap.h"

// (不需要包含 nclgl/Mesh.h，因为 B_Mesh.cpp 已经处理了)

namespace NCLGL_Impl {

    B_Heightmap::B_Heightmap(Mesh* nclMesh)
        : B_Mesh(nclMesh) { // 3. (规范) 将 nclMesh 实例传递给父类 B_Mesh 的构造函数
        
        // B_Mesh 的构造函数会处理 nclMesh 的所有权和检查
        
        // Draw() 
        // 方法已由父类 B_Mesh 自动实现
    }

    // 注意：如果 I_Heightmap 接口 
    // 中有额外的方法（如 GetVertex），你需要在这里实现它们。
    // 目前，它只是一个标记接口，继承 B_Mesh 已满足所有要求。

} // namespace NCLGL_Impl