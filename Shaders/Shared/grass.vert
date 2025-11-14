#version 460 core

layout(location = 0) in vec4 inInstance; // xyz: world position, w: random seed

out VS_OUT {
    vec3 worldPos;
    float noise;
} vsOut;

void main() {
    vsOut.worldPos = inInstance.xyz;
    vsOut.noise = inInstance.w;
}