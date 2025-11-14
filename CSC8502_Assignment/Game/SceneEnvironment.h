/**
* @file SceneEnvironment.h
 * @brief 定义场景渲染所需的环境数据。
 * @details
 * SceneEnvironment 结构体封装了渲染器在切换不同场景时所需的外部资源与光照参数。
 * 其中包含天空盒纹理、方向光数据以及基础的场景颜色，用于驱动 Renderer 的环境配置。
 */
#pragma once

#include <memory>

#include "../Core/Light.h"
#include "../Engine/IAL/I_Texture.h"

struct SceneEnvironment {
    std::shared_ptr<Engine::IAL::I_Texture> skyboxTexture;
    std::shared_ptr<Engine::IAL::I_Texture> grassBaseColorTexture;
    Light directionalLight;
    Vector3 sceneColour;
};