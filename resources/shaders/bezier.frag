#version 430 core

in VertexData{
//    vec2 mTexCoord;
    vec4 fColor;
} VertexIn;

layout(location = 0) out vec4 diffuseColor;

void main(void)
{
    diffuseColor = VertexIn.fColor;
}