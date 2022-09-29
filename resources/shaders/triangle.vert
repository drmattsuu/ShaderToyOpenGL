#version 450 core

layout(location = 0) in vec3 i_vertexPosition;

layout(location = 0) uniform vec4 u_color;

out vec4 frag_color;

void main()
{
    frag_color = u_color;
    gl_Position = vec4(i_vertexPosition, 1.0);
}
