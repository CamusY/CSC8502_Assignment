#version 460 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 uModel;
uniform mat4 uViewProj;

out vec2 vTexCoord;

void main() {
    vec4 worldPosition = uModel * vec4(position, 1.0);
    vTexCoord = texCoord;
    gl_Position = uViewProj * worldPosition;
}