#version 460 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 uModel;
uniform mat4 uViewProj;

out vec2 vTexCoord;
out vec3 vWorldPos;
out vec3 vNormal;

void main() {
    vec4 worldPosition = uModel * vec4(position, 1.0);
    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    vTexCoord = texCoord;
    vWorldPos = worldPosition.xyz;
    vNormal = normalize(normalMatrix * normal);
    gl_Position = uViewProj * worldPosition;
}