#version 430

layout (location = 0) in vec3 i_vertexPosition;
layout (location = 1) in vec3 i_vertexNormal;
layout (location = 2) in vec2 i_vertexUV;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 CamNormal;
    vec3 CamEyeDir;
    vec3 CamLightDir;
} vs_out;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_perspective;
uniform vec3 u_cameraPos;
uniform vec3 u_lightPos;

void main()
{
    gl_Position = u_perspective * u_view * u_model * vec4(i_vertexPosition, 1.0);

    vs_out.TexCoords = i_vertexUV;
    // Position of the vertex, in worldspace : M * position
    vs_out.FragPos = (u_model * vec4(i_vertexPosition, 1)).xyz;
    
    // Vector that goes from the vertex to the camera, in camera space.
    // In camera space, the camera is at the origin (0,0,0).
    vec3 camVertPos = (u_view * u_model * vec4(i_vertexPosition, 1)).xyz;
    vs_out.CamEyeDir = vec3(0, 0, 0) - camVertPos;

    // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
    vec3 camLightPos = (u_view * vec4(u_lightPos, 1)).xyz;
    vs_out.CamLightDir = camLightPos + vs_out.CamEyeDir;
    
    // Normal of the the vertex, in camera space
    // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
    vs_out.CamNormal = (u_view * u_model * vec4(i_vertexNormal, 0)).xyz;
}
