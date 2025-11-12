#version 460 core
in vec2 vTexCoord;
in vec3 vWorldPos;

uniform sampler2D uReflectionTex;
uniform sampler2D uRefractionTex;
uniform vec3 uCameraPos;
uniform vec3 uLightColor;
uniform vec3 uAmbientColor;

out vec4 fragColor;

void main() {
    vec3 normal = normalize(vec3(0.0, 1.0, 0.0));
    vec3 viewDir = normalize(uCameraPos - vWorldPos);
    float fresnel = clamp(1.0 - max(dot(viewDir, normal), 0.0), 0.0, 1.0);
    vec2 reflectionUV = vec2(vTexCoord.x, 1.0 - vTexCoord.y);
    vec3 reflection = texture(uReflectionTex, reflectionUV).rgb;
    vec3 refraction = texture(uRefractionTex, vTexCoord).rgb;
    vec3 lighting = uAmbientColor + uLightColor * 0.2;
    vec3 color = mix(refraction, reflection, fresnel * 0.7 + 0.2);
    color += lighting * 0.1;
    fragColor = vec4(color, 0.75);
}