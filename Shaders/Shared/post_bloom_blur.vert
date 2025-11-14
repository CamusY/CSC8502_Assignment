#version 460 core
in vec2 vUV;

out vec4 fragColor;

uniform sampler2D uScene;
uniform float uThreshold;

void main() {
    vec3 color = texture(uScene, vUV).rgb;
    float brightness = max(max(color.r, color.g), color.b);
    vec3 result = (brightness > uThreshold) ? color : vec3(0.0);
    fragColor = vec4(result, 1.0);
}