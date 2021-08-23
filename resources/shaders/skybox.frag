#version 430
layout (location = 0) out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube texture_skybox;

void main()
{
    FragColor = texture(texture_skybox, TexCoords);
}
