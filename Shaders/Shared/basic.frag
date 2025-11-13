#version 460 core
in vec3 vColor;
in vec3 vWorldPos;

out vec4 fragColor;

uniform vec3 uLightPosition;
uniform vec3 uLightColor;
uniform vec3 uAmbientColor;
uniform mat4 uShadowMatrix;
uniform sampler2DShadow uShadowMap;
uniform float uShadowStrength;

float EvaluateShadow(vec3 worldPos) {
    if (uShadowStrength <= 0.0) {
        return 1.0;
    }
    vec4 shadowCoord = uShadowMatrix * vec4(worldPos, 1.0);
    if (shadowCoord.w <= 0.0) {
        return 1.0;
    }
    vec3 projCoords = shadowCoord.xyz / shadowCoord.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 1.0;
    }
    float bias = 0.0015;
    float sample1 = texture(uShadowMap, vec3(projCoords.xy, projCoords.z - bias));
    return mix(1.0, sample1, clamp(uShadowStrength, 0.0, 1.0));
}
void main() {
    vec3 ambient = uAmbientColor * vColor;
    vec3 normal = vec3(0.0, 1.0, 0.0);
    vec3 lightDir = normalize(uLightPosition - vWorldPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = uLightColor * vColor * diff;
    float shadow = EvaluateShadow(vWorldPos);
    vec3 color = ambient + shadow * diffuse;
    fragColor = vec4(color, 1.0);
}