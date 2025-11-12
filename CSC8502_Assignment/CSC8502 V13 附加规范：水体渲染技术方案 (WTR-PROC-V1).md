### CSC8502 V13 附加规范：水体渲染技术方案 (WTR-PROC-V1)

文档状态： 草案

相关需求 ID： P-0 (水体), P-7 (光照)

---

### 1.0 范围与目的

本文档是 `CSC8502 务实计划 V13` 的附加技术规范。其目的在于详细定义项目需求 `P-0` (水体渲染) 的高级实现方案。

本规范的核心目标是：**放弃**使用静态法线贴图 (如 `Assets/Textures/water_normal.png`)，转而采用**程序化法线 (Procedural Normals)** 技术。此技术将在片段着色器 (`water.frag`) 中实时计算水面法线，以实现高保真、动态的波浪效果，同时严格遵守 V13 计划定义的 IAL 架构和光栅化管线。

### 2.0 核心架构约束

所有 `P-0` (水体) 的实现**必须**遵守 V13 计划中定义的核心架构约束：

1. **几何基础 (P-6):** 水体几何体**必须**是一个通过 `Engine::IAL::I_ResourceFactory::CreateQuad()` 接口创建的 `I_Mesh` 实例。
    
2. **场景图 (P-6):** 该 `I_Mesh` **必须**作为 `SceneNode` 插入 `SceneGraph` (场景图) 中进行管理和渲染。
    
3. **渲染管线 (P-0):** 水体**必须**利用 `Renderer` 模块生成的 FBO（帧缓冲对象）进行平面反射 (Planar Reflection) 和折射 (Refraction)。
    
4. **光照 (P-7):** 水体表面**必须**能正确接收符合 `P-7` (Blinn-Phong) 规范的动态光照。
    

### 3.0 技术方案：程序化法线

#### 3.1 概述

本方案将在 `water.frag` 片段着色器中，通过移植和改编 "Seascape" (TDM, Shadertoy) 的噪声函数，程序化地生成一个动态的高度场 (Height Field)，并使用**有限差分 (Finite Differences)** 方法实时推导出每个片段的法线向量。

#### 3.2 概念来源 (Conceptual Basis)

- **借鉴内容：** "Seascape" 中的 `hash()`、`noise()`、`sea_octave()` 噪声函数，以及 `SEA_FREQ`、`SEA_SPEED`、`octave_m` 等用于 FBM (Fractional Brownian Motion) 的常量和数学逻辑。
    
- **拒绝内容：** **严禁**使用 "Seascape" 中的 `map()`、`heightMapTracing()` 或任何光线行进 (Ray-Marching) 相关的几何生成逻辑。本方案**必须**在 V13 的光栅化管线 (Rasterization Pipeline) 上执行。
    

### 4.0 规范：组件实现

#### 4.1 Renderer 模块 (P-0, P-7)

`Renderer` 模块在渲染水体节点 (Water Node) 之前，其职责规范如下：

1. **FBO 绑定:** **必须**按 `Sprint 2, Day 12` 规范，准备好 `u_ReflectionFBO` 和 `u_RefractionFBO` 纹理，并将其绑定到可用的纹理单元。
    
2. **Uniform 传递:** **必须**向 `water.frag` 着色器传递以下 `uniform` 变量：
    
    - `uniform sampler2D u_ReflectionFBO;` (反射 FBO)
        
    - `uniform sampler2D u_RefractionFBO;` (折射 FBO)
        
    - `uniform float u_Time;` (来自 `I_GameTimer`，用于驱动动画)
        
    - `uniform vec3 u_CameraPos;` (世界空间相机位置)
        
    - `uniform vec3 u_LightPos;` (世界空间光源位置, 满足 P-7)
        

#### 4.2 water.vert (顶点着色器规范)

1. **输入 (In):**
    
    - `in vec3 a_Position;` (来自 `CreateQuad()` 的本地顶点坐标)
        
2. **Uniforms (In):**
    
    - `uniform mat4 u_ModelMatrix;`
        
    - `uniform mat4 u_ViewMatrix;`
        
    - `uniform mat4 u_ProjMatrix;`
        
3. **输出 (Out - Varyings):**
    
    - `out vec3 v_WorldPos;` (片段的世界空间坐标，用于光照)
        
    - `out vec2 v_WorldXZ;` (片段的世界空间 `(x, z)` 坐标，用于噪声采样)
        
    - `out vec4 v_ScreenPos;` (片段的裁剪空间坐标，用于 FBO 采样)
        

#### 4.3 water.frag (片段着色器规范)

`water.frag` 的实现**必须**遵循以下逻辑管线：

1. **4.3.1 噪声函数库 (Noise Function Library)**
    
    - **规范:** **必须**从 "Seascape" 源码中完整移植 `hash(vec2 p)` 和 `noise(in vec2 p)` 函数。
        
    - **规范:** **必须**移植 `sea_octave(vec2 uv, float choppy)` 函数，作为核心的 FBM 噪声生成器。
        
    - **规范:** **必须**定义 `SEA_FREQ`, `SEA_SPEED`, `SEA_CHOPPY`, `octave_m` 等常量，并确保 `u_Time` 通过 `SEA_TIME` 宏 正确参与计算。
        
2. **4.3.2 高度场函数 (Height Field Function)**
    
    - **规范:** **必须**实现 `float getHeight(vec2 uv)` 函数。
        
    - **实现:** 此函数**必须**调用 4.3.1 中的 `sea_octave`，并（根据 TDM 逻辑）迭代 `ITER_FRAGMENT` 次（建议初始值为 `3` 以平衡性能）来累加高度值。
        
    - **输入:** `vec2 uv` (来自 `v_WorldXZ`)。
        
3. **4.3.3 法线计算函数 (Normal Calculation Function)**
    
    - **规范:** **必须**实现 `vec3 getProceduralNormal(vec2 uv)` 函数。
        
    - **实现:** **必须**采用**有限差分 (Finite Differences)** 方法。
        
    - **流程:**
        
        1. 定义一个微小偏移量 `epsilon` (例如 `0.01`)。
            
        2. 调用 `h_center = getHeight(uv)`。
            
        3. 调用 `h_x = getHeight(vec2(uv.x + epsilon, uv.y))`。
            
        4. 调用 `h_z = getHeight(vec2(uv.x, uv.y + epsilon))`。
            
        5. **必须**使用 `vec3 normal = normalize(vec3(h_center - h_x, epsilon, h_center - h_z))` 来计算法线。
            
4. **4.3.4 渲染管线 (Main Function Pipeline)**
    
    - `main()` 函数的执行**必须**遵循以下顺序：
        
    - **1. (计算法线):** `vec3 normal = getProceduralNormal(v_WorldXZ);`
        
    - **2. (计算向量):** `vec3 viewDir = normalize(u_CameraPos - v_WorldPos);` `vec3 lightDir = normalize(u_LightPos - v_WorldPos);`
        
    - **3. (计算菲涅尔 Fresnel):** `float fresnelFactor = pow(1.0 - dot(viewDir, normal), 5.0);`
        
    - **4. (计算 FBO 扰动):** 使用 `normal.xz` 分量创建 `vec2 distortion` 扰动向量。
        
    - **5. (计算 FBO 坐标):** `vec2 screenUV = (v_ScreenPos.xy / v_ScreenPos.w) * 0.5 + 0.5;` `vec2 reflectUV = screenUV + distortion;` `vec2 refractUV = screenUV + distortion;`
        
    - **6. (采样 FBO):** `vec3 reflectColor = texture(u_ReflectionFBO, reflectUV).rgb;` `vec3 refractColor = texture(u_RefractionFBO, refractUV).rgb;`
        
    - **7. (混合颜色):** `vec3 waterColor = mix(refractColor, reflectColor, fresnelFactor);` (可选项：进一步与 `SEA_BASE_COLOR` 混合以获得水体底色)。
        
    - **8. (计算光照 P-7):** `vec3 halfVec = normalize(lightDir + viewDir);` `float specFactor = pow(max(dot(normal, halfVec), 0.0), 64.0);` `vec3 specular = vec3(1.0) * specFactor;`
        
    - **9. (合成输出):** `fragColor = vec4(waterColor + specular, 1.0);`
        

### 5.0 风险与缓解

1. **风险 (性能):**
    
    - `getHeight` 中的 FBM 迭代 ( `ITER_FRAGMENT`) 计算量巨大，可能导致 GPU 瓶颈，违反 `NFR-5` (60fps) 需求。
        
    - **缓解:** `ITER_FRAGMENT` **必须**作为着色器内的常量，在 `Sprint 2` 集成测试时，从低值 (`3`) 开始调优，在视觉效果和性能 (`NFR-5`) 之间找到平衡点。
        
2. **风险 (视觉):**
    
    - 程序化噪声可能在远处产生高频振荡（闪烁）。
        
    - **缓解:** 可接受的风险。高级缓解（如基于距离的 Mipmapping 或分析性抗锯齿）超出了 `CSC8502` 的范围。
        

---