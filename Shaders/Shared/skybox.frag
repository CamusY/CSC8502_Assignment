#version 460 core
in vec3 vDirection;

uniform samplerCube uSkybox;

out vec4 fragColor;

vec3 SRGBToLinear(vec3 c) {
    return pow(c, vec3(2.2));
}

void main() {
    vec3 sampled = texture(uSkybox, normalize(vDirection)).rgb;
    fragColor = vec4(SRGBToLinear(sampled), 1.0);
}