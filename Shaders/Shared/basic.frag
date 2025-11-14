#version 460 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBitangent;
in vec2 vTexCoord;
in vec3 vViewPos;

out vec4 fragColor;

uniform vec3 uLightPosition;
uniform vec3 uLightColor;
uniform vec3 uAmbientColor;
uniform vec3 uCameraPos;
uniform mat4 uShadowMatrix;
uniform sampler2DShadow uShadowMap;
uniform float uShadowStrength;
uniform vec3 uFogColor;
uniform float uFogDensity;

uniform sampler2D uBaseColorMap;
uniform sampler2D uNormalMap;
uniform sampler2D uMetallicRoughnessMap;
uniform sampler2D uAOMap;
uniform sampler2D uEmissiveMap;
uniform samplerCube uEnvironmentMap;

uniform int uHasBaseColorMap;
uniform int uHasNormalMap;
uniform int uHasMetallicRoughnessMap;
uniform int uHasAOMap;
uniform int uHasEmissiveMap;
uniform int uUseEnvironment;

uniform int uDebugMode;
uniform float uNearPlane;
uniform float uFarPlane;

uniform vec4 uBaseColorFactor;
uniform float uMetallicFactor;
uniform float uRoughnessFactor;
uniform vec3 uEmissiveFactor;
uniform float uAlphaCutoff;
uniform int uAlphaMode; // 0 opaque, 1 mask, 2 blend
uniform int uDoubleSided;
uniform float uEnvironmentIntensity;
uniform float uEnvironmentMaxLod;

const float PI = 3.14159265359;

vec3 SRGBToLinear(vec3 c) {
    return pow(c, vec3(2.2));
}

vec3 LinearToSRGB(vec3 c) {
    return pow(max(c, vec3(0.0)), vec3(1.0 / 2.2));
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / max(PI * denom * denom, 1e-4);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / max(NdotV * (1.0 - k) + k, 1e-4);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

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
    float bias = max(0.002 * (1.0 - dot(normal, lightDir)), 0.0005);
    float sample1 = texture(uShadowMap, vec3(projCoords.xy, projCoords.z - bias));
    return mix(1.0, sample1, clamp(uShadowStrength, 0.0, 1.0));
}

mat3 BuildTBN() {
    vec3 T = normalize(vTangent);
    vec3 B = normalize(vBitangent);
    vec3 N = normalize(vNormal);
    return mat3(T, B, N);
}

vec3 GetNormal(vec3 defaultNormal) {
    if (uHasNormalMap == 1) {
        vec3 tangentNormal = texture(uNormalMap, vTexCoord).xyz * 2.0 - 1.0;
        mat3 TBN = BuildTBN();
        return normalize(TBN * tangentNormal);
    }
    return normalize(defaultNormal);
}

void main() {
    vec3 viewDir = normalize(uCameraPos - vWorldPos);
    vec3 normal = GetNormal(vNormal);
    if (uDoubleSided == 1 && dot(normal, viewDir) < 0.0) {
        normal = -normal;
    }

    if (uDebugMode == 1) {
        vec3 debugNormal = normalize(normal) * 0.5 + 0.5;
        fragColor = vec4(debugNormal, 1.0);
        return;
    }

    if (uDebugMode == 2) {
        float depth = gl_FragCoord.z;
        float linearDepth = (2.0 * uNearPlane) / (uFarPlane + uNearPlane - depth * (uFarPlane - uNearPlane));
        float normalized = clamp(linearDepth / uFarPlane, 0.0, 1.0);
        fragColor = vec4(vec3(normalized), 1.0);
        return;
    }

    vec4 baseSample = vec4(1.0);
    if (uHasBaseColorMap == 1) {
        baseSample = texture(uBaseColorMap, vTexCoord);
    }
    vec3 baseColor = SRGBToLinear(baseSample.rgb) * uBaseColorFactor.rgb;
    float alpha = baseSample.a * uBaseColorFactor.a;

    if (uAlphaMode == 1 && alpha < uAlphaCutoff) {
        discard;
    }

    float metallic = clamp(uMetallicFactor, 0.0, 1.0);
    float roughness = clamp(uRoughnessFactor, 0.04, 1.0);
    if (uHasMetallicRoughnessMap == 1) {
        vec4 mr = texture(uMetallicRoughnessMap, vTexCoord);
        metallic *= mr.b;
        roughness *= mr.g;
    }
    roughness = clamp(roughness, 0.04, 1.0);

    float ao = (uHasAOMap == 1) ? texture(uAOMap, vTexCoord).r : 1.0;

    vec3 emissive = uEmissiveFactor;
    if (uHasEmissiveMap == 1) {
        emissive *= SRGBToLinear(texture(uEmissiveMap, vTexCoord).rgb);
    }

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, baseColor, metallic);

    vec3 lightVector = uLightPosition - vWorldPos;
    float distance = length(lightVector);
    vec3 lightDir = normalize(lightVector);
    vec3 radiance = uLightColor / max(distance * distance, 1e-4);

    vec3 halfDir = normalize(lightDir + viewDir);
    float NDF = DistributionGGX(normal, halfDir, roughness);
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);
    vec3 F = FresnelSchlick(max(dot(halfDir, viewDir), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = max(4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0), 1e-4);
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = (kD * baseColor / PI) * NdotL;
    vec3 directLighting = (diffuse + specular) * radiance;
    float shadow = EvaluateShadow(vWorldPos, normal);

    vec3 ambient = uAmbientColor * baseColor * ao;
    if (uUseEnvironment == 1) {
        vec3 R = reflect(-viewDir, normal);
        float mip = roughness * uEnvironmentMaxLod;
        vec3 irradiance = textureLod(uEnvironmentMap, normal, uEnvironmentMaxLod).rgb;
        vec3 prefiltered = textureLod(uEnvironmentMap, R, mip).rgb;
        vec3 fresnel = FresnelSchlick(max(dot(normal, viewDir), 0.0), F0);
        vec3 envSpecular = prefiltered * fresnel;
        vec3 envDiffuse = irradiance * baseColor;
        ambient = (envDiffuse * (1.0 - metallic) + envSpecular) * ao * uEnvironmentIntensity;
    }

    vec3 color = ambient + shadow * directLighting + emissive;

    float fogDistance = length(uCameraPos - vWorldPos);
    float fogFactor = clamp(exp(-pow(fogDistance * uFogDensity, 2.0)), 0.0, 1.0);
    vec3 foggedColor = mix(uFogColor, color, fogFactor);

    vec3 finalColor = LinearToSRGB(foggedColor);
    float finalAlpha = (uAlphaMode == 2) ? clamp(alpha, 0.0, 1.0) : 1.0;
    fragColor = vec4(finalColor, finalAlpha);
}