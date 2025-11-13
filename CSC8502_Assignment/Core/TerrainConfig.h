/**
* @file TerrainConfig.h
 * @brief 共享的地形尺度与高度图配置常量。
 */
#pragma once

#include "nclgl/Vector3.h"

inline constexpr float kHeightmapResolution = 1024.0f;
inline  const Vector3 kTerrainScale = Vector3(4.0f, 0.4f, 4.0f);
inline const float kTerrainExtent = kHeightmapResolution * kTerrainScale.x;
inline const float kTerrainHalfExtent = kTerrainExtent * 0.5f;