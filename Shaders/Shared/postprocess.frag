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

vec3 ComposeScene() {
    vec3 sceneColor = texture(uScene, vUV).rgb;
    vec3 bloomColor = texture(uBloom, vUV).rgb;
    if (uDisplayMode == 1) {
        return bloomColor;
    }
    if (uDisplayMode == 2) {
        return sceneColor;
    }
    return ToneMap(sceneColor + bloomColor);
}

void main() {
    fragColor = vec4(ComposeScene(), 1.0);
}