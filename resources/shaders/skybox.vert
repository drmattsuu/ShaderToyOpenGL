#version 450
layout (location = 0) in vec3 i_vertexPosition;

out vec3 TexCoords;

uniform mat4 u_view;
uniform mat4 u_perspective;

void main()
{
    TexCoords = i_vertexPosition;
    vec4 pos = u_perspective * u_view * vec4(i_vertexPosition, 1.0);
    gl_Position = pos.xyww;
}
