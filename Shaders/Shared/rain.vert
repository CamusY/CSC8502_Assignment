#version 460 core

layout(location = 0) in vec2 aCorner;
layout(location = 1) in vec3 aInstancePosition;
layout(location = 2) in float aInstanceSpeed;

uniform mat4 uView;
uniform mat4 uProj;
uniform vec3 uCameraPos;
uniform vec3 uCameraRight;
uniform vec3 uRainDirection;
uniform float uMinSpeed;
uniform float uMaxSpeed;
uniform float uMinLength;
uniform float uMaxLength;
uniform float uWidth;

out float vHeightFactor;
out float vCornerFactor;
out vec3 vWorldPos;

float saturate(float value) {
    return clamp(value, 0.0, 1.0);
}

void main() {
    float speedRange = max(uMaxSpeed - uMinSpeed, 1e-3);
    float speedFactor = saturate((aInstanceSpeed - uMinSpeed) / speedRange);
    float lengthValue = mix(uMinLength, uMaxLength, speedFactor);
    vec3 horizontal = uCameraRight * (aCorner.x * uWidth);
    vec3 vertical = normalize(uRainDirection) * (aCorner.y * lengthValue);
    vec3 worldPosition = aInstancePosition + horizontal + vertical;
    vHeightFactor = speedFactor;
    vCornerFactor = aCorner.y;
    vWorldPos = worldPosition;
    gl_Position = uProj * uView * vec4(worldPosition, 1.0);
}