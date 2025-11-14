#version 460 core

in float vHeightFactor;
in float vCornerFactor;
in vec3 vWorldPos;

out vec4 fragColor;

uniform vec3 uBaseColor;
uniform vec3 uFogColor;
uniform float uFogDensity;
uniform vec3 uCameraPos;

float saturate(float value) {
    return clamp(value, 0.0, 1.0);
}

void main() {
    float alpha = mix(0.08, 0.32, vCornerFactor);
    vec3 baseTint = mix(uBaseColor * (0.6 + 0.4 * vHeightFactor), vec3(1.0), vCornerFactor * 0.25);
    float fogDistance = length(uCameraPos - vWorldPos);
    float fogFactor = saturate(exp(-pow(fogDistance * uFogDensity, 2.0)));
    vec3 color = mix(uFogColor, baseTint, fogFactor);
    fragColor = vec4(color, alpha);
}