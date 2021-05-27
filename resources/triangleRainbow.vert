#version 430 core

layout(location = 0) in vec3 i_vertexPosition;
layout(location = 1) in vec4 i_vertexColor;

out vec4 frag_color;

void main()
{
    frag_color = i_vertexColor;
    gl_Position.xyz = i_vertexPosition;
    gl_Position.w = 1.0;
}
