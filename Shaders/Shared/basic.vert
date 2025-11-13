#version 460 core
layout(location = 0) in vec3 position;

uniform mat4 uModel;
uniform mat4 uViewProj;
uniform vec3 uColor;

out vec3 vColor;
out vec3 vWorldPos;

void main() {
    vec4 worldPosition = uModel * vec4(position, 1.0);
    vWorldPos = worldPosition.xyz;
    vColor = uColor;
    gl_Position = uViewProj * worldPosition;
}