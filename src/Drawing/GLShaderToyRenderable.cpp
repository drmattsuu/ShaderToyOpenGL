#include "GLShaderToyRenderable.h"

#include "Util/FileUtils.h"
#include "Util/ShaderUtils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "imgui.h"

#include <filesystem>
#include <algorithm>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

namespace
{
constexpr char kFragShaderHeader[] = R"V0G0N(
#version 430

layout (location = 0) out vec4 FragColor;

layout(location = 0) uniform vec3 iResolution;               // viewport resolution (in pixels)
layout(location = 1) uniform float iTime;                    // shader playback time (in seconds)
layout(location = 2) uniform float iTimeDelta;               // render time (in seconds)
layout(location = 3) uniform int iFrame;                     // shader playback frame
//uniform float iChannelTime[4];        // channel playback time (in seconds)
//uniform vec3 iChannelResolution[4];   // channel resolution (in pixels)
layout(location = 4) uniform vec4 iMouse;                  // mouse pixel coords. xy: current (if MLB down), zw: click
//uniform samplerXX iChannel0..3;       // input channel. XX = 2D/Cube
//uniform vec4 iDate;                   // (year, month, day, time in seconds)
//uniform float iSampleRate;            // sound sample rate (i.e., 44100)
)V0G0N";
// todo : implement the rest of ShaderToy's uniforms

constexpr char kFragShaderFooter[] = R"V0G0N(
void main()
{
    vec4 color = vec4(0.0,0.0,0.0,1.0);
    mainImage(color, gl_FragCoord.xy);
    color.w = 1.0;
    FragColor = color;
}
)V0G0N";

// clang-format off
const static float kQuadVertices[] = {
    -1.0f,  1.0f,
    -1.0f, -1.0f,
     1.0f, -1.0f,

    -1.0f,  1.0f,
     1.0f, -1.0f,
     1.0f,  1.0f,
};
// clang-format on

static const ShaderToyShader kDefaultShader{"resources/shaders/ShaderToy/default.ShaderToy"};
static const std::string kShaderToyVert = "resources/shaders/shadertoy.vert";
}  // namespace

GLShaderToyRenderable::GLShaderToyRenderable(const int* displaySize)
    : GLRenderable("ShaderToy"), m_displaySize(displaySize)
{
    m_shaders.push_back(kDefaultShader);
}

void GLShaderToyRenderable::Init()
{
    glGenVertexArrays(1, &m_quadVertexArrayObject);
    glGenBuffers(1, &m_quadVertexBufferObject);
    glBindVertexArray(m_quadVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kQuadVertices), &kQuadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    LocateShaders();
}

void GLShaderToyRenderable::CleanGLResources()
{
    if (m_quadVertexBufferObject)
        glDeleteBuffers(1, &m_quadVertexBufferObject);
    if (m_quadVertexArrayObject)
        glDeleteVertexArrays(1, &m_quadVertexArrayObject);

    m_quadVertexArrayObject = m_quadVertexBufferObject = 0;

    ClearShaders();
}

void GLShaderToyRenderable::NewFrame(float deltaT)
{
    m_deltaTime = 0.0f;
    if (!m_paused)
    {
        m_frame++;
        m_deltaTime = (deltaT * 0.001f);
        m_runTime += (deltaT * 0.001f);
    }

    ImGui::SetNextWindowPos(ImVec2(300.f, 10.f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("ShaderToy", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (ImGui::Button("Reset"))
        {
            ResetTime();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reload Shaders"))
        {
            ClearShaders();
            LocateShaders();
            ResetTime();
        }

        const char* pauseText = m_paused ? "Resume" : "Pause";
        if(ImGui::Button(pauseText))
        {
            m_paused = !m_paused;
        }

        for (int i = 0; i < m_shaders.size(); ++i)
        {
            const ShaderToyShader& shader = m_shaders[i];
            fs::path shaderToyPath(shader.FilePath);

            std::string name = shaderToyPath.filename().string();

            if (ImGui::RadioButton(name.c_str(), &m_currentShader, i))
            {
                ResetTime();
            }
        }
    }
    ImGui::End();

    if (m_quadVertexArrayObject == 0 || m_quadVertexBufferObject == 0)
        Init();

    ShaderToyShader& currentShader = m_shaders[m_currentShader];

    if (currentShader.ShaderId == 0)
    {
        currentShader.ShaderId = ConstructShaderToyShader(currentShader.FilePath);
    }
}

void GLShaderToyRenderable::Render()
{
    ShaderToyShader& currentShader = m_shaders[m_currentShader];

    if (currentShader.ShaderId == 0)
    {
        return;
    }

    bool prevEnableDepthTest = glIsEnabled(GL_DEPTH_TEST);

    glDisable(GL_DEPTH_TEST);

    glUseProgram(currentShader.ShaderId);

    glUniform3f(0, static_cast<float>(m_displaySize[0]), static_cast<float>(m_displaySize[1]), 0.0f);
    glUniform1f(1, m_runTime);
    glUniform1f(2, m_deltaTime);
    glUniform1i(3, m_frame);

    glBindVertexArray(m_quadVertexArrayObject);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glUseProgram(0);

    if (prevEnableDepthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

GLuint GLShaderToyRenderable::ConstructShaderToyShader(std::string filePath)
{
    std::string vertSrc = LoadFileText(kShaderToyVert);
    std::string fragSrc = kFragShaderHeader + LoadFileText(filePath) + kFragShaderFooter;

    if (vertSrc.empty() || fragSrc.empty())
    {
        return 0;
    }

    GLuint vertShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    CompileShaderSrc(vertShaderId, vertSrc);
    CompileShaderSrc(fragShaderId, fragSrc);

    GLuint programId = glCreateProgram();
    LinkShader(programId, vertShaderId, fragShaderId);

    glDeleteShader(vertShaderId);
    glDeleteShader(fragShaderId);

    return programId;
}

void GLShaderToyRenderable::ClearShaders()
{
    for (const ShaderToyShader& shader : m_shaders)
    {
        if (shader.ShaderId != 0)
            glDeleteProgram(shader.ShaderId);
    }

    m_shaders.clear();

    m_shaders.push_back(kDefaultShader);
}

void GLShaderToyRenderable::LocateShaders()
{
    fs::path shaderToyShaderDir("resources/shaders/ShaderToy");

    if (!fs::is_directory(shaderToyShaderDir))
    {
        std::cout << "Unable to open resources/shaders/ShaderToy" << std::endl;
        return;
    }

    fs::directory_iterator itr(shaderToyShaderDir);
    fs::directory_iterator end;
    for (itr; itr != end; ++itr)
    {
        fs::path currentFile = itr->path();
        auto found = std::find_if(m_shaders.begin(), m_shaders.end(), [&currentFile](const ShaderToyShader& shader) {
            return fs::path(shader.FilePath).filename().string() == currentFile.filename().string();
        });

        if (found != m_shaders.end())
        {
            continue;
        }

        ShaderToyShader newShader = {currentFile.string()};
        m_shaders.push_back(newShader);
    }
}

void GLShaderToyRenderable::ResetTime()
{
    m_frame = 0;
    m_runTime = 0.0f;
}
