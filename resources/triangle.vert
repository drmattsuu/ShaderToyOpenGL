#version 430 core

layout(location = 0) in vec3 i_vertexPosition;

void main()
{
    gl_Position.xyz = i_vertexPosition;
    gl_Position.w = 1.0;
}
