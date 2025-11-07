/**
* @file I_Heightmap.h
 * @brief 定义了地形（Heightmap）的抽象接口。
 * @details
 * 该文件定义了一个专门用于地形的接口，以满足 (P-0) 基础场景的需求。
 *
 * 架构说明（V13）：
 * 此接口继承自 `I_Mesh`，
 * 意味着地形可以像其他任何网格一样被场景图（SceneGraph） 
 * 存储并通过 `Draw()` 方法绘制。
 *
 * (NFR-11.3) 
 * `nclgl` 库本身没有 `Heightmap` 类 
 * 或 `.raw` 加载器。
 * 轨道 B 的实现 `B_Heightmap` 
 * 将包装一个 `nclgl::Mesh`，
 * 而 `B_Factory` 
 * 则负责自研 `.raw` 文件的加载逻辑，
 * 并使用 `nclgl::Mesh` 的 `protected` 成员
 * 来填充顶点数据。
 *
 * (NFR-1) 规范约束：本文件严禁包含 nclgl/Mesh.h。
 * (NFR-2) 规范要求：本文件被授权且必须包含 nclgl 数学库（Vector3.h）。
 *
 * @see I_Mesh
 * @see I_ResourceFactory::LoadHeightmap
 *
 * @class Engine::IAL::I_Heightmap
 * @brief 地形网格的纯虚接口。
 * @details 继承自 I_Mesh，使其可被渲染器直接绘制。
 *
 * @fn Engine::IAL::I_Heightmap::~I_Heightmap
 * @brief 虚拟析构函数。
 *
 * @fn Engine::IAL::I_Heightmap::GetVertex
 * @brief (可选功能) 
 * 获取地形上特定网格坐标的精确世界空间顶点位置。
 * @details
 * 此功能可用于物理、碰撞检测或在地形上动态放置对象。
 * (NFR-2) 此函数签名必须直接使用 nclgl::Vector3 类型。
 * @param x 地形网格的 x 坐标索引。
 * @param y (或 z) 地形网格的 y/z 坐标索引。
 * @return 该点的 nclgl::Vector3 世界空间坐标。
 */

#pragma once

// (NFR-2) 接口直接依赖 nclgl 通用数学库
#include "nclgl/Vector3.h"

// IAL 依赖 IAL (纯净)
#include "IAL/I_Mesh.h"

namespace Engine::IAL {
    class I_Heightmap : public virtual I_Mesh {
    public:
        virtual ~I_Heightmap() {}

        // virtual Vector3 GetVertex(int x, int y) = 0;
    };

}
