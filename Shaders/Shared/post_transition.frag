#version 460 core
in vec2 vUV;

out vec4 fragColor;

uniform sampler2D uScene;
uniform sampler2D uBloom;
uniform float uTimer;
uniform int uDisplayMode;
uniform float uExposure;

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

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
    float progress = clamp(uTimer, 0.0, 1.0);
    vec3 sceneColor = ComposeScene();
    float radius = length(vUV - vec2(0.5));
    float ring = smoothstep(progress * 1.4 - 0.25, progress * 1.4, radius);
    float noise = hash(vUV * 64.0);
    float dissolve = smoothstep(progress - 0.2, progress + 0.05, noise + (1.0 - ring) * 0.4);
    vec3 fadeColor = mix(vec3(0.05, 0.05, 0.08), vec3(0.9, 0.85, 0.8), progress);
    vec3 finalColor = mix(fadeColor, sceneColor, dissolve);
    fragColor = vec4(finalColor, 1.0);
}