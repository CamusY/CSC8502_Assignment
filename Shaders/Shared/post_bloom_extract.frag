#version 460 core

in vec2 vUV;

out vec4 fragColor;

uniform sampler2D uScene;
uniform float uThreshold;

void main() {
    vec3 color = texture(uScene, vUV).rgb;
    float brightness = max(max(color.r, color.g), color.b);
    float soft = clamp((brightness - uThreshold) / max(uThreshold, 1e-4), 0.0, 1.0);
    vec3 bloom = color * soft;
    fragColor = vec4(bloom, 1.0);
}