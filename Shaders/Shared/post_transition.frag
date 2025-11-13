#version 460 core
in vec2 vUV;

out vec4 fragColor;

uniform sampler2D uScene;
uniform float uTimer;

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    float progress = clamp(uTimer, 0.0, 1.0);
    vec4 sceneColor = texture(uScene, vUV);
    float radius = length(vUV - vec2(0.5));
    float ring = smoothstep(progress * 1.4 - 0.25, progress * 1.4, radius);
    float noise = hash(vUV * 64.0);
    float dissolve = smoothstep(progress - 0.2, progress + 0.05, noise + (1.0 - ring) * 0.4);
    vec3 fadeColor = mix(vec3(0.05, 0.05, 0.08), vec3(0.9, 0.85, 0.8), progress);
    vec3 finalColor = mix(fadeColor, sceneColor.rgb, dissolve);
    fragColor = vec4(finalColor, 1.0);
}