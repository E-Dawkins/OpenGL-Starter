#version 410 core

uniform sampler2D opaqueDepthTex;   // opaque depth
uniform sampler2D previousDepthTex; // depth texture from previous peel
uniform vec2 viewportSize;

in vec2 Uv;

out vec4 FragColor;

void main() {
    vec2 screenUv = gl_FragCoord.xy / viewportSize;
    float opaqueZ = texture(opaqueDepthTex, screenUv).r;

    float currZ = gl_FragCoord.z;

    // discard occluded fragments
    if (currZ >= opaqueZ + 1e-5)
        discard;

    float prevZ = texture(previousDepthTex, screenUv).r;

    // discard fragments that are *not* deeper than last peel layer
    if (currZ <= prevZ + 1e-5)
        discard;

    FragColor = vec4(1.0, 0.5, 0.2, 0.5); // translucent orange
}