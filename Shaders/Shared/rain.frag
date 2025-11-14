#version 460 core

in float vHeightFactor;
in float vCornerFactor;
in vec3 vWorldPos;

out vec4 fragColor;

uniform vec3 uBaseColor;
uniform vec3 uFogColor;
uniform float uFogDensity;
uniform vec3 uCameraPos;
uniform float uFarPlane;

float ComputeCurvedFogFactor(vec3 toCamera, float density, float farPlane) {
    float distance = length(toCamera);
    if (distance <= 1e-3) {
        return 1.0;
    }
    vec3 direction = toCamera / distance;
    float horizonFactor = 1.0 - abs(direction.y);
    horizonFactor = horizonFactor * horizonFactor;
    float safeFar = max(farPlane, 1e-3);
    float curvatureDistance = distance + horizonFactor * (distance * distance) / safeFar;
    float baseFog = clamp(exp(-pow(curvatureDistance * density, 2.0)), 0.0, 1.0);
    float normalized = clamp(curvatureDistance / safeFar, 0.0, 1.0);
    float clipFade = smoothstep(0.72, 0.98, normalized);
    return clamp(baseFog * (1.0 - clipFade), 0.0, 1.0);
}

void main() {
    float alpha = mix(0.08, 0.32, vCornerFactor);
    vec3 baseTint = mix(uBaseColor * (0.6 + 0.4 * vHeightFactor), vec3(1.0), vCornerFactor * 0.25);
    vec3 toCamera = uCameraPos - vWorldPos;
    float fogFactor = ComputeCurvedFogFactor(toCamera, uFogDensity, uFarPlane);
    vec3 color = mix(uFogColor, baseTint, fogFactor);
    fragColor = vec4(color, alpha);
}