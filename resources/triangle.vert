#version 430 core

layout(location = 0) in vec3 i_vertexPosition;

layout(location = 0) uniform vec4 u_color;

out vec4 frag_color;

void main()
{
    frag_color = u_color;
    gl_Position.xyz = i_vertexPosition;
    gl_Position.w = 1.0;
}
