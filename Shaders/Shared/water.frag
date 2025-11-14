#version 460 core
in vec2 vTexCoord;     // 只是从 mesh 传进来，不用于 refraction
in vec3 vWorldPos;

uniform sampler2D      uReflectionTex;
uniform sampler2D      uRefractionTex;

uniform vec3           uCameraPos;
uniform vec3           uLightColor;
uniform vec3           uAmbientColor;

uniform mat4           uReflectionViewProj;

uniform mat4           uShadowMatrix;
uniform sampler2DShadow uShadowMap;
uniform float          uShadowStrength;

uniform vec3  uFogColor;
uniform float uFogDensity;

uniform int   uDebugMode;
uniform float uNearPlane;
uniform float uFarPlane;

out vec4 fragColor;

float ComputeCurvedFogFactor(vec3 toCamera, float density, float farPlane) {
    float distance = length(toCamera);
    if (distance <= 1e-3) {
        return 1.0;
    }
    vec3 direction = toCamera / distance;
    float horizonFactor = 1.0 - abs(direction.y);
    horizonFactor = horizonFactor * horizonFactor;
    float safeFar = max(farPlane, 1e-3);
    float curvatureDistance = distance + horizonFactor * (distance * distance) / safeFar;
    float baseFog = clamp(exp(-pow(curvatureDistance * density, 2.0)), 0.0, 1.0);
    float normalized = clamp(curvatureDistance / safeFar, 0.0, 1.0);
    float clipFade = smoothstep(0.72, 0.98, normalized);
    return clamp(baseFog * (1.0 - clipFade), 0.0, 1.0);
}

// =============== 影子计算 ===============
float EvaluateShadow(vec3 worldPos) {
    if (uShadowStrength <= 0.0) return 1.0;

    vec4 shadowCoord = uShadowMatrix * vec4(worldPos, 1.0);
    if (shadowCoord.w <= 0.0) return 1.0;

    vec3 projCoords = shadowCoord.xyz / shadowCoord.w;
    projCoords = projCoords * 0.5 + 0.5;

    // 超界不采样影子
    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
    projCoords.y < 0.0 || projCoords.y > 1.0)
    return 1.0;

    float bias = 0.0015;
    float shadow = texture(uShadowMap, vec3(projCoords.xy, projCoords.z - bias));
    return mix(1.0, shadow, clamp(uShadowStrength, 0.0, 1.0));
}


// =============== 主渲染 ===============
void main() {

    // ----------------------
    // Fresnel
    // ----------------------
    vec3 normal = normalize(vec3(0,1,0));
    vec3 viewDir = normalize(uCameraPos - vWorldPos);
    float fresnel = pow(1.0 - max(dot(viewDir, normal), 0.0), 3.0);

    // ----------------------
    // 反射 UV —— 用 reflectionViewProj 投影
    // ----------------------
    vec4 clip = uReflectionViewProj * vec4(vWorldPos, 1.0);

    vec2 reflectionUV = vec2(0.5);
    bool outOfBounds = true;

    if (abs(clip.w) > 0.00001) {
        vec2 ndc = clip.xy / clip.w;       // NDC -1~1
        reflectionUV = ndc * 0.5 + 0.5;    // 转 0~1

        // 不再强制翻转 Y（你原来的 bug 就在这里）
        outOfBounds = (
        reflectionUV.x < 0.0 || reflectionUV.x > 1.0 ||
        reflectionUV.y < 0.0 || reflectionUV.y > 1.0
        );
    }

    // ----------------------
    // 反射采样（超界则 clamp 防止重复天空盒）
    // ----------------------
    vec2 uvClamped = clamp(reflectionUV, 0.0, 1.0);
    vec3 reflection = texture(uReflectionTex, outOfBounds ? uvClamped : reflectionUV).rgb;

    // ----------------------
    // 折射 UV —— 必须用屏幕 UV，而不是 mesh UV
    // 原 water.frag 里用 vTexCoord 是完全错误的！
    // ----------------------
    vec2 screenUV = gl_FragCoord.xy / textureSize(uRefractionTex, 0);
    vec3 refraction = texture(uRefractionTex, screenUV).rgb;

    // ----------------------
    // 光照
    // ----------------------
    vec3 lighting = uAmbientColor + uLightColor * 0.2;

    // ----------------------
    // Fresnel 混合反射/折射
    // ----------------------
    vec3 color = mix(refraction, reflection, fresnel * 0.7 + 0.2);

    // ----------------------
    // 影子
    // ----------------------
    float shadow = EvaluateShadow(vWorldPos);
    color = mix(color * 0.6, color, shadow);

    // ----------------------
    // 最终输出
    // ----------------------
    color += lighting * 0.1;

    float distanceToCamera = length(uCameraPos - vWorldPos);
    float distanceNorm = clamp(distanceToCamera / 600.0, 0.0, 1.0);
    float reflectBoost = 0.5 * distanceNorm;
    float fresnelMix = clamp(fresnel * 0.7 + 0.2 + reflectBoost, 0.0, 1.0);
    vec3  surfaceColor = mix(uFogColor, color, fresnelMix);

    float fogFactor = ComputeCurvedFogFactor(uCameraPos - vWorldPos, uFogDensity, uFarPlane);
    vec3  outCol = mix(uFogColor, surfaceColor, fogFactor);

    if (uDebugMode == 1) {
        vec3 normal = vec3(0.0, 1.0, 0.0);
        fragColor = vec4(normal * 0.5 + 0.5, 1.0);
        return;
    }
    if (uDebugMode == 2) {
        float depth = gl_FragCoord.z;
        float linearDepth = (2.0 * uNearPlane * uFarPlane)
        / (uFarPlane + uNearPlane - depth * (uFarPlane - uNearPlane));
        float normalized = clamp((linearDepth - uNearPlane) / (uFarPlane - uNearPlane), 0.0, 1.0);
        fragColor = vec4(vec3(normalized), 1.0);
        return;
    }
    
    fragColor = vec4(outCol,0.75);
}
