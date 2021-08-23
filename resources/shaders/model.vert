#version 430

layout (location = 0) in vec3 i_vertexPosition;
layout (location = 1) in vec3 i_vertexNormal;
layout (location = 2) in vec2 i_vertexUV;
layout (location = 3) in vec3 i_tangent;
layout (location = 4) in vec3 i_bitangent;  

out VS_OUT {
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_perspective;

uniform vec3 u_lightPos;
uniform vec3 u_cameraPos;

void main()
{
    vec3 FragPos = vec3(u_model * vec4(i_vertexPosition, 1.0));   
    vs_out.TexCoords = i_vertexUV;

    mat3 normalMatrix = transpose(inverse(mat3(u_model)));
    vec3 T = normalize(normalMatrix * i_tangent);
    vec3 N = normalize(normalMatrix * i_vertexNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));    
    vs_out.TangentLightPos = TBN * u_lightPos;
    vs_out.TangentViewPos  = TBN * u_cameraPos;
    vs_out.TangentFragPos  = TBN * FragPos;

    gl_Position = u_perspective * u_view * u_model * vec4(i_vertexPosition, 1.0);
}
