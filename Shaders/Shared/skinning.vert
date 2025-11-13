#version 460 core
layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;
layout(location = 5) in vec4 weights;
layout(location = 6) in ivec4 joints;

uniform mat4 uModel;
uniform mat4 uViewProj;
uniform vec4 uClipPlane;
uniform mat4 uBoneMatrices[128];
uniform int uBoneCount;

out vec2 vTexCoord;
out vec3 vWorldPos;
out vec3 vNormal;

mat4 ComputeSkinMatrix() {
    if (uBoneCount <= 0) {
        return mat4(1.0);
    }
    mat4 skin = mat4(0.0);
    float weightSum = 0.0;
    for (int i = 0; i < 4; ++i) {
        int jointIndex = joints[i];
        float weight = weights[i];
        if (jointIndex >= 0 && jointIndex < uBoneCount && weight > 0.0) {
            skin += weight * uBoneMatrices[jointIndex];
            weightSum += weight;
        }
    }
    float remaining = max(0.0, 1.0 - weightSum);
    return skin + remaining * mat4(1.0);
}

void main() {
    mat4 skinMatrix = ComputeSkinMatrix();
    mat4 modelSkin = uModel * skinMatrix;
    vec4 worldPosition = modelSkin * vec4(position, 1.0);
    mat3 normalMatrix = transpose(inverse(mat3(modelSkin)));

    vTexCoord = texCoord;
    vWorldPos = worldPosition.xyz;
    vNormal = normalize(normalMatrix * normal);

    gl_Position = uViewProj * worldPosition;
    gl_ClipDistance[0] = dot(worldPosition, uClipPlane);
}