#version 460 core
in vec3 vDirection;

uniform samplerCube uSkybox;

out vec4 fragColor;

void main() {
    fragColor = texture(uSkybox, normalize(vDirection));
}