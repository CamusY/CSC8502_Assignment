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

out vec4 fragColor;

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

    vec3 color = ambient + diffuse + specular;
    fragColor = vec4(color, 1.0);
}