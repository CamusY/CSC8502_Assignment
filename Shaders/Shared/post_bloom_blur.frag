#version 460 core
in vec2 vUV;

out vec4 fragColor;

uniform sampler2D uImage;
uniform int uHorizontal;

const float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
    vec2 texOffset = 1.0 / vec2(textureSize(uImage, 0));
    vec3 result = texture(uImage, vUV).rgb * weights[0];
    for (int i = 1; i < 5; ++i) {
        vec2 offset = (uHorizontal == 1)
        ? vec2(texOffset.x * float(i), 0.0)
        : vec2(0.0, texOffset.y * float(i));
        result += texture(uImage, vUV + offset).rgb * weights[i];
        result += texture(uImage, vUV - offset).rgb * weights[i];
    }
    fragColor = vec4(result, 1.0);
}