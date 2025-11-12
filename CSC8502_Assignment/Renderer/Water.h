/**
* @file Water.h
 * @brief 声明用于管理水体节点与参数的封装类。
 * @details
 * Water 类负责通过资源工厂创建全屏四边形网格，将其旋转和平移到场景中的水面高度，
 * 同时保留水面尺寸与高度信息，供渲染器在多次渲染传递中复用。该类仅负责场景节点
 * 的构建与维护，具体的反射与折射帧缓冲由 Renderer 在 Day12 阶段进行管理。
 */
#pragma once

#include <memory>

#include "../Core/SceneGraph.h"
#include "../Engine/IAL/I_ResourceFactory.h"

#include "nclgl/Vector2.h"
#include "nclgl/Vector3.h"

class Water {
public:
    Water(const std::shared_ptr<Engine::IAL::I_ResourceFactory>& factory,
          float height,
          const Vector2& size);
    ~Water();

    std::shared_ptr<SceneNode> GetNode() const;
    float GetHeight() const;
    Vector2 GetSize() const;

private:
    void ConfigureNode(const Vector2& size);

    std::shared_ptr<Engine::IAL::I_ResourceFactory> m_factory;
    std::shared_ptr<SceneNode> m_node;
    float m_height;
    Vector2 m_size;
};