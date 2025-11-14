#version 460 core

in GS_OUT {
    vec2 texCoord;
    vec3 worldPos;
} fsIn;

out vec4 fragColor;

uniform vec3 uCameraPos;
uniform vec3 uColor;

void main() {
    float heightBlend = clamp(fsIn.texCoord.y, 0.0, 1.0);
    vec3 baseColor = uColor * mix(0.7, 1.0, heightBlend);
    float distanceToCamera = length(uCameraPos - fsIn.worldPos);
    float fogFactor = clamp(exp(-0.0006 * distanceToCamera * distanceToCamera), 0.0, 1.0);
    vec3 fogColor = vec3(0.25, 0.32, 0.38);
    vec3 finalColor = mix(fogColor, baseColor, fogFactor);
    float alpha = mix(0.6, 0.85, heightBlend);
    fragColor = vec4(finalColor, alpha);
}