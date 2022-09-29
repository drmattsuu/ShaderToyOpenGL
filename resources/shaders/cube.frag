#version 450

// Interpolated values from the vertex shaders
in vec2 frag_uv;

// Ouput data
layout (location = 0) out vec4 o_color;

// Values that stay constant for the whole mesh.
uniform sampler2D u_texSampler;

void main()
{
    // Output color = color of the texture at the specified UV
    o_color = texture(u_texSampler, frag_uv);
}