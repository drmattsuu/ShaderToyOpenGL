#version 450 core

in VertexData{
    vec4 fColor;
} VertexIn;

layout(location = 0) out vec4 diffuseColor;

void main(void)
{
    diffuseColor = VertexIn.fColor;
}