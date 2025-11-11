#version 460 core
layout(location = 0) in vec3 position;

uniform mat4 uModel;
uniform mat4 uViewProj;
uniform vec3 uColor;

out vec3 vColor;

void main() {
    vColor = uColor;
    gl_Position = uViewProj * uModel * vec4(position, 1.0);
}