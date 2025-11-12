/**
* @file Light.h
 * @brief 定义渲染管线使用的基础光源数据结构。
 * @details
 * 该结构体封装了位置、颜色与环境光分量，所有字段均直接使用 nclgl 数学类型。
 * Renderer 在 Day11 阶段会将此数据上传至着色器，实现 Blinn-Phong 光照计算。
 */
#pragma once

#include "nclgl/Vector3.h"

struct Light {
    Vector3 position;
    Vector3 color;
    Vector3 ambient;
};