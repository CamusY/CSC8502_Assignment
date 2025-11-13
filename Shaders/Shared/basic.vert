#version 460 core
layout(location = 0) in vec3 position;
layout(location = 3) in vec3 normal;


uniform mat4 uModel;
uniform mat4 uViewProj;
uniform vec4 uClipPlane;
uniform vec3 uColor;

out vec3 vColor;
out vec3 vWorldPos;
out vec3 vNormal;

void main() {
    vec4 worldPosition = uModel * vec4(position, 1.0);
    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    vWorldPos = worldPosition.xyz;
    vColor = uColor;
    vNormal = normalize(normalMatrix * normal);
    gl_Position = uViewProj * worldPosition;
    gl_ClipDistance[0] = dot(worldPosition, uClipPlane);
}