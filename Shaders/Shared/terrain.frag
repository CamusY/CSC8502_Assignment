#version 460 core
in vec2 vTexCoord;
in vec3 vWorldPos;
in vec3 vNormal;

uniform sampler2D uDiffuse;
uniform vec3 uLightPosition;
uniform vec3 uLightColor;
uniform vec3 uAmbientColor;
uniform vec3 uCameraPos;
uniform float uSpecularPower;
uniform mat4 uShadowMatrix;
uniform sampler2DShadow uShadowMap;
uniform float uShadowStrength;

out vec4 fragColor;

float EvaluateShadow(vec3 worldPos, vec3 normal) {
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
    vec3 lightDir = normalize(uLightPosition - worldPos);
    float bias = max(0.0015 * (1.0 - dot(normal, lightDir)), 0.0005);
    float sample1 = texture(uShadowMap, vec3(projCoords.xy, projCoords.z - bias));
    return mix(1.0, sample1, clamp(uShadowStrength, 0.0, 1.0));
}

void main() {
    vec3 albedo = texture(uDiffuse, vTexCoord).rgb;
    vec3 N = normalize(vNormal);
    vec3 L = normalize(uLightPosition - vWorldPos);
    vec3 V = normalize(uCameraPos - vWorldPos);
    vec3 H = normalize(L + V);

    float diff = max(dot(N, L), 0.0);
    float spec = diff > 0.0 ? pow(max(dot(N, H), 0.0), uSpecularPower) : 0.0;

    vec3 ambient = uAmbientColor * albedo;
    vec3 diffuse = diff * uLightColor * albedo;
    vec3 specular = spec * uLightColor;

    float shadow = EvaluateShadow(vWorldPos, N);
    vec3 color = ambient + shadow * (diffuse + specular);
    fragColor = vec4(color, 1.0);
}