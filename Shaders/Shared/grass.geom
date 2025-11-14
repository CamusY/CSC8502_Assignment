#version 460 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 uViewProj;
uniform float uTime;

in VS_OUT {
    vec3 worldPos;
    float noise;
} gsIn[];

out GS_OUT {
    vec2 texCoord;
    vec3 worldPos;
} gsOut;

const float PI = 3.1415926;

void EmitBladeVertex(vec3 position, vec2 uv) {
    gsOut.texCoord = uv;
    gsOut.worldPos = position;
    gl_Position = uViewProj * vec4(position, 1.0);
    EmitVertex();
}

void main() {
    vec3 base = gsIn[0].worldPos;
    float seed = gsIn[0].noise;
    float height = mix(1.2, 2.4, fract(seed * 3.73));
    float sway = sin(uTime * 0.8 + seed * 12.37) * 0.3;

    float angle = fract(seed * 7.91) * PI * 2.0;
    vec3 dir = normalize(vec3(cos(angle), 0.0, sin(angle)));
    vec3 widthOffset = dir * 0.35;
    vec3 topOffset = widthOffset * 0.25;

    vec3 apex = base + vec3(0.0, height, 0.0);

    float baseInfluence = 0.15;
    float tipInfluence = 1.0;

    vec3 baseLeft = base - widthOffset + dir * (sway * baseInfluence);
    vec3 baseRight = base + widthOffset + dir * (sway * baseInfluence);
    vec3 topRight = apex + topOffset + dir * (sway * tipInfluence);
    vec3 topLeft = apex - topOffset + dir * (sway * tipInfluence);

    EmitBladeVertex(baseLeft, vec2(0.0, 0.0));
    EmitBladeVertex(baseRight, vec2(1.0, 0.0));
    EmitBladeVertex(topRight, vec2(1.0, 1.0));
    EmitBladeVertex(topLeft, vec2(0.0, 1.0));
    EndPrimitive();
}