#version 460 core
layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec4 tangent;

uniform mat4 uModel;
uniform mat4 uViewProj;
uniform mat4 uView;
uniform vec4 uClipPlane;

out vec3 vWorldPos;
out vec3 vNormal;
out vec3 vTangent;
out vec3 vBitangent;
out vec2 vTexCoord;
out vec3 vViewPos;

void main() {
    vec4 worldPosition = uModel * vec4(position, 1.0);
    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    vec3 N = normalize(normalMatrix * normal);
    vec3 T = normalize(normalMatrix * tangent.xyz);
    vec3 B = normalize(cross(N, T) * tangent.w);

    vWorldPos = worldPosition.xyz;
    vNormal = N;
    vTangent = T;
    vBitangent = B;
    vTexCoord = texCoord;
    vec4 viewPosition = uView * worldPosition;
    vViewPos = viewPosition.xyz;

    gl_Position = uViewProj * worldPosition;
    gl_ClipDistance[0] = dot(worldPosition, uClipPlane);
}