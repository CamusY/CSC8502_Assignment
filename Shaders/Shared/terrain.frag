#version 460 core
in vec2 vTexCoord;

uniform sampler2D uDiffuse;

out vec4 fragColor;

void main() {
    fragColor = texture(uDiffuse, vTexCoord);
}