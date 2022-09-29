#version 450 core

layout(location = 0) in vec2 i_vertexPosition;
layout(location = 1) in vec3 i_vertexColor;

out VertexData{
    vec4 color;
    flat vec3 startPos;
    vec3 vertPos;
} VertexOut;


void main()
{
    VertexOut.color = vec4(i_vertexColor, 1.0);
    vec4 pos = vec4(i_vertexPosition.x, i_vertexPosition.y, 0.0, 1.0);
    gl_Position = pos;
    VertexOut.vertPos = pos.xyz;
    VertexOut.startPos = VertexOut.vertPos;
}
