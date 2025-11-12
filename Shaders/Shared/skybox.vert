#version 460 core
layout(location = 0) in vec3 position;

uniform mat4 uViewProj;

out vec3 vDirection;

void main() {
    vec4 clipPos = uViewProj * vec4(position, 1.0);
    gl_Position = clipPos.xyww;
    vDirection = position;
}