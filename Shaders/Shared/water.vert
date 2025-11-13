// water.vert 片段（新增 vScreenUV）
#version 460 core
layout(location=0) in vec3 position;
layout(location=2) in vec2 texCoord;

uniform mat4 uModel;
uniform mat4 uViewProj;

out vec2 vTexCoord;
out vec3 vWorldPos;
out vec2 vScreenUV;

void main() {
    vec4 worldPosition = uModel * vec4(position, 1.0);
    vWorldPos = worldPosition.xyz;
    vTexCoord = texCoord;

    vec4 clip = uViewProj * worldPosition;
    vec2 ndc = clip.xy / max(clip.w, 1e-6);
    vScreenUV = ndc * 0.5 + 0.5;

    gl_Position = clip;
}
