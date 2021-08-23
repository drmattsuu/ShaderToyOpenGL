#include "GLShaderToyRenderable.h"

namespace
{

constexpr char k_FragShaderHeader[] = R"V0G0N(
#version 430

layout (location = 0) out vec4 FragColor;

uniform vec3 iResolution;               // viewport resolution (in pixels)
uniform float iTime;                    // shader playback time (in seconds)
uniform float iTimeDelta;               // render time (in seconds)
uniform int iFrame;                     // shader playback frame
//uniform float iChannelTime[4];        // channel playback time (in seconds)
//uniform vec3 iChannelResolution[4];   // channel resolution (in pixels)
//uniform vec4 iMouse;                  // mouse pixel coords. xy: current (if MLB down), zw: click
//uniform samplerXX iChannel0..3;       // input channel. XX = 2D/Cube
//uniform vec4 iDate;                   // (year, month, day, time in seconds)
//uniform float iSampleRate;            // sound sample rate (i.e., 44100)
)V0G0N";
// todo : implement the rest of ShaderToy's uniforms

constexpr char k_FragShaderFooter[] = R"V0G0N(
void main()
{
    vec4 color = vec4(0.0,0.0,0.0,1.0);
    mainImage(color, gl_FragCoord.xy);
    color.w = 1.0;
    FragColor = color;
}
)V0G0N";
}

GLShaderToyRenderable::GLShaderToyRenderable() : GLRenderable("ShaderToy")
{
}

void GLShaderToyRenderable::Init()
{
}

void GLShaderToyRenderable::CleanGLResources()
{
}

void GLShaderToyRenderable::NewFrame(float deltaT)
{
    m_runtime += deltaT;
}

void GLShaderToyRenderable::Render()
{
}
