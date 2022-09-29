#version 450

layout(location = 0) in vec3 i_vertexPosition;

void main()
{
    gl_Position = vec4(i_vertexPosition, 1.0);
}
