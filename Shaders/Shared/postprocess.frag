#version 460 core
in vec2 vUV;

out vec4 fragColor;

uniform sampler2D uScene;
uniform sampler2D uBloom;
uniform int uDisplayMode;
uniform float uExposure;

vec3 ToneMap(vec3 color) {
    return vec3(1.0) - exp(-color * max(uExposure, 0.01));
}

vec3 LinearToSRGB(vec3 color) {
    return pow(max(color, vec3(0.0)), vec3(1.0 / 2.2));
}

vec3 ComposeScene() {
    vec3 sceneColor = max(texture(uScene, vUV).rgb, vec3(0.0));
    vec3 bloomColor = max(texture(uBloom, vUV).rgb, vec3(0.0));
    if (uDisplayMode == 1) {
        return ToneMap(bloomColor * 2.5);
    }
    if (uDisplayMode == 2) {
        return ToneMap(sceneColor);
    }
    return ToneMap(sceneColor + bloomColor);
}

void main() {
    vec3 color = ComposeScene();
    fragColor = vec4(LinearToSRGB(color), 1.0);
}