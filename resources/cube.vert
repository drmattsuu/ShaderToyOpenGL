#version 430

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 i_vertexPosition;
layout(location = 1) in vec2 i_vertexUV;

// Output data ; will be interpolated for each fragment.
out vec2 frag_uv;

// Values that stay constant for the whole mesh.
uniform mat4 u_mvp;

void main(){

    // Output position of the vertex, in clip space : perspective * view * model * position
    gl_Position =  u_mvp * vec4(i_vertexPosition, 1);
    
    // UV of the vertex.
    frag_uv = i_vertexUV;
}

