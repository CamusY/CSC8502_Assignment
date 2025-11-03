#pragma once

// NFR-2: 接口直接依赖 nclgl 通用数学库
#include "nclgl/Vector3.h"

// IAL 依赖 IAL (纯净)
#include "IAL/I_Mesh.h"

// NFR-1: 严禁 #include "nclgl/Mesh.h"

namespace Engine::IAL {

    /**
     * @brief IAL 高度图接口 (P-0)
     * @details
     * - 继承自 I_Mesh，因为它是一种可绘制的特殊网格。
     * - NFR-11.3: nclgl 库没有 Heightmap 类，B_Heightmap (轨道 B) 将是一个自研了 RAW 加载器、内部填充并包装了 nclgl::Mesh 的适配器。
     * - GetVertex: (可选功能) 允许 Demo 层的物理或交互系统查询地形上某点的世界坐标。
     * NFR-2 规范要求此函数签名必须使用 nclgl::Vector3。
     */
    class I_Heightmap : public I_Mesh {
    public:
        virtual ~I_Heightmap() {}
        
        // (可选功能: 用于物理或对象放置)
        // virtual Vector3 GetVertex(int x, int y) const = 0; 
    };
    
} // namespace Engine::IAL