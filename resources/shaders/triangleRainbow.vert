#version 450 core

layout(location = 0) in vec3 i_vertexPosition;
layout(location = 1) in vec4 i_vertexColor;

out vec4 frag_color;

void main()
{
    frag_color = i_vertexColor;
    gl_Position = vec4(i_vertexPosition, 1.0);
}
