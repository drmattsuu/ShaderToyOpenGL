#version 450 core

in VertexData
{
    vec4 fColor;
    flat vec3 fStartPos;
    vec3 fVertPos;
} VertexIn;


layout (location = 0) out vec4 fragColor;

layout (location = 1) uniform vec2 Viewport;
layout (location = 2) uniform float dashSize = 4;
layout (location = 3) uniform float gapSize = 6;

void main(void)
{
    vec2 dir = (VertexIn.fVertPos.xy - VertexIn.fStartPos.xy) * Viewport/2.0;
    float dist = length(dir);

    bool toDiscard = fract(dist / (dashSize + gapSize)) > dashSize/(dashSize + gapSize);
    float alpha = mix(VertexIn.fColor.w, 0.0f, toDiscard);
    fragColor = vec4(VertexIn.fColor.xyz, alpha); 
}