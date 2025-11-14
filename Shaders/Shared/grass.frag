#version 460 core

in GS_OUT {
    vec2 texCoord;
    vec3 worldPos;
} fsIn;

out vec4 fragColor;

uniform vec3 uCameraPos;
uniform vec3 uColor;
uniform sampler2D uBaseColorMap;
uniform sampler2D uAlphaShapeMap;
uniform bool uHasBaseColorMap;
uniform bool uHasAlphaShapeMap;
uniform float uFallbackAlpha;

void main() {
    vec2 uv = clamp(fsIn.texCoord, 0.0, 1.0);
    vec3 sampledColor = texture(uBaseColorMap, uv).rgb;
    float baseMix = uHasBaseColorMap ? 1.0 : 0.0;
    sampledColor = mix(uColor, sampledColor, baseMix);
    float sampledAlpha = texture(uAlphaShapeMap, uv).r;
    sampledAlpha = uHasAlphaShapeMap ? sampledAlpha : uFallbackAlpha;

    float heightBlend = clamp(uv.y, 0.0, 1.0);
    vec3 gradient = mix(vec3(0.85, 0.95, 0.8), vec3(1.0, 1.0, 1.0), heightBlend);
    vec3 baseColor = sampledColor * gradient;
    float distanceToCamera = length(uCameraPos - fsIn.worldPos);
    float fogFactor = clamp(exp(-0.0006 * distanceToCamera * distanceToCamera), 0.0, 1.0);
    vec3 fogColor = vec3(0.25, 0.32, 0.38);
    vec3 finalColor = mix(fogColor, baseColor, fogFactor);
    fragColor = vec4(finalColor, sampledAlpha);
}