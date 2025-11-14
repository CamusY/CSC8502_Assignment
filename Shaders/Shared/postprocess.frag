#version 460 core
in vec2 vUV;

out vec4 fragColor;

uniform sampler2D uScene;
uniform sampler2D uBloom;
uniform int uDisplayMode;
uniform float uExposure;
uniform float uViewportAspect;
uniform float uCircleEdge;

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
        vec3 base = ToneMap(sceneColor);
        vec3 highlight = ToneMap(bloomColor * 1.8);
        return clamp(base * 0.45 + highlight, 0.0, 1.0);
    }
    if (uDisplayMode == 2) {
        return ToneMap(sceneColor);
    }
    return ToneMap(sceneColor + bloomColor);
}

float CircularMask(vec2 uv) {
    vec2 centered = uv * 2.0 - 1.0;
    if (uViewportAspect > 1.0) {
        centered.x /= uViewportAspect;
    }
    else {
        centered.y /= max(uViewportAspect, 1e-4);
    }
    float radius = length(centered);
    float edge = clamp(uCircleEdge, 0.0, 0.5);
    return 1.0 - smoothstep(1.0 - edge, 1.0, radius);
}

void main() {
    vec3 color = ComposeScene();
    float mask = CircularMask(vUV);
    fragColor = vec4(LinearToSRGB(color) * mask, 1.0);
}