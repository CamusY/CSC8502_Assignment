#version 460 core
in vec2 vTexCoord;
in vec3 vWorldPos;

uniform sampler2D uReflectionTex;
uniform sampler2D uRefractionTex;
uniform vec3 uCameraPos;
uniform vec3 uLightColor;
uniform vec3 uAmbientColor;
uniform mat4 uReflectionViewProj;
uniform mat4 uShadowMatrix;
uniform sampler2DShadow uShadowMap;
uniform float uShadowStrength;

out vec4 fragColor;

float EvaluateShadow(vec3 worldPos) {
    if (uShadowStrength <= 0.0) {
        return 1.0;
    }
    vec4 shadowCoord = uShadowMatrix * vec4(worldPos, 1.0);
    if (shadowCoord.w <= 0.0) {
        return 1.0;
    }
    vec3 projCoords = shadowCoord.xyz / shadowCoord.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 1.0;
    }
    float bias = 0.0015;
    float sample1 = texture(uShadowMap, vec3(projCoords.xy, projCoords.z - bias));
    return mix(1.0, sample1, clamp(uShadowStrength, 0.0, 1.0));
}

void main() {
    vec3 normal = normalize(vec3(0.0, 1.0, 0.0));
    vec3 viewDir = normalize(uCameraPos - vWorldPos);
    float fresnel = clamp(1.0 - max(dot(viewDir, normal), 0.0), 0.0, 1.0);
    vec4 reflectionClip = uReflectionViewProj * vec4(vWorldPos, 1.0);
    vec2 reflectionUV = vec2(0.5);
    if (abs(reflectionClip.w) > 0.00001) {
        vec2 ndc = reflectionClip.xy / reflectionClip.w;
        reflectionUV = ndc * 0.5 + 0.5;
    }
    reflectionUV.y = 1.0 - reflectionUV.y;
    bool reflectionOutOfBounds = reflectionClip.w <= 0.0
    || reflectionUV.x < 0.0 || reflectionUV.x > 1.0
    || reflectionUV.y < 0.0 || reflectionUV.y > 1.0;
    vec2 clampedReflectionUV = clamp(reflectionUV, vec2(0.0), vec2(1.0));
    vec3 reflection = reflectionOutOfBounds
    ? texture(uReflectionTex, clampedReflectionUV).rgb
    : texture(uReflectionTex, reflectionUV).rgb;
    vec3 refraction = texture(uRefractionTex, vTexCoord).rgb;
    vec3 lighting = uAmbientColor + uLightColor * 0.2;
    vec3 color = mix(refraction, reflection, fresnel * 0.7 + 0.2);
    float shadow = EvaluateShadow(vWorldPos);
    color = mix(color * 0.6, color, shadow);
    color += lighting * 0.1;
    fragColor = vec4(color, 0.75);
}