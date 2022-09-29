#version 450 core

layout(location = 0) in vec2 i_vertexPosition;
layout(location = 1) in vec3 i_vertexColor;

out VertexData{
    vec4 color;
} VertexOut;

void main()
{
    VertexOut.color = vec4(i_vertexColor, 1.0);
    gl_Position = vec4(i_vertexPosition.x, i_vertexPosition.y, 0.0, 1.0);
}
