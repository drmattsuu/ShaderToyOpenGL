#include "GLRenderable.h"

#include "GLInputManager.h"
#include "Util/ShaderUtils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include <string>

GLRenderable::~GLRenderable()
{
    auto it = m_subscribedEvents.begin();
    auto end = m_subscribedEvents.end();
    for (it; it != end; it++)
    {
         GLInputManager::GetInputManager()->UnsubscribeEvent(it->first, it->second);
    }
    m_subscribedEvents.clear();
}

EventDelegatePtr GLRenderable::SubscribeEvent(EventType e)
{
    if (m_subscribedEvents[e])
    {
        // todo : warn event already subscribed to this slot
        UnsubscribeEvent(e);
    }

    auto result = GLInputManager::GetInputManager()->SubscribeEvent(e);
    m_subscribedEvents[e] = result;
    return result;
}

void GLRenderable::UnsubscribeEvent(EventType e)
{
    auto it = m_subscribedEvents.find(e);
    auto end = m_subscribedEvents.end();

    if (it != end)
        GLInputManager::GetInputManager()->UnsubscribeEvent(it->first, it->second);
}

GLuint GLRenderable::LoadShader(const std::string& vertPath, const std::string& fragPath)
{
    return LoadShaderFile(vertPath, fragPath);
}

void GLHelloTriangle::Init()
{
    glGenVertexArrays(1, &m_vertexArrayId);
    glBindVertexArray(m_vertexArrayId);

    m_shaderId = LoadShader("resources/triangle.vert", "resources/triangle.frag");

    glGenBuffers(1, &m_vertexBufferId);
}

void GLHelloTriangle::CleanGLResources()
{
    if (m_vertexBufferId)
        glDeleteBuffers(1, &m_vertexBufferId);
    if (m_vertexArrayId)
        glDeleteVertexArrays(1, &m_vertexArrayId);
    if (m_shaderId)
        glDeleteProgram(m_shaderId);

    m_vertexBufferId = 0;
    m_vertexArrayId = 0;
    m_shaderId = 0;
}

void GLHelloTriangle::NewFrame(float deltaT)
{
    if (!m_shaderId)
    {
        Init();
    }

    ImGui::SetNextWindowPos(ImVec2(300.f, 10.f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(m_name.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (ImGui::CollapsingHeader("Triangle Vertex Buffer"))
        {
            ImGui::InputFloat3("0", &m_bufData[0]);
            ImGui::InputFloat3("1", &m_bufData[3]);
            ImGui::InputFloat3("2", &m_bufData[6]);
        }
        bool expanded = ImGui::CollapsingHeader("Triangle Color");
        ImGui::SameLine();
        ImGui::ColorButton("TriangleColorButton", ImVec4(m_color[0], m_color[1], m_color[2], m_color[3]));
        if (expanded)
        {
            ImGui::ColorPicker4("Triangle Color", m_color);
        }
    }
    ImGui::End();
}

void GLHelloTriangle::Render()
{
    glBindVertexArray(m_vertexArrayId);

    glUseProgram(m_shaderId);

    // populate u_color data
    glUniform4fv(0, 1, m_color);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_bufData), m_bufData, GL_DYNAMIC_DRAW);
    // populate i_vertexPosition data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);

    // clear our VBO selection and GL state so future calls to GL don't pollute our data here.
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GLRainbowTriangle::Init()
{
    glGenVertexArrays(1, &m_vertexArrayId);
    glBindVertexArray(m_vertexArrayId);

    m_shaderId = LoadShader("resources/triangleRainbow.vert", "resources/triangle.frag");

    glGenBuffers(1, &m_vertexBufferId);

    glGenBuffers(1, &m_colorBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_bufColorData), m_bufColorData, GL_STATIC_DRAW);
}

void GLRainbowTriangle::CleanGLResources()
{
    if (m_colorBufferId)
        glDeleteBuffers(1, &m_colorBufferId);
    if (m_vertexBufferId)
        glDeleteBuffers(1, &m_vertexBufferId);
    if (m_vertexArrayId)
        glDeleteVertexArrays(1, &m_vertexArrayId);
    if (m_shaderId)
        glDeleteProgram(m_shaderId);

    m_colorBufferId = 0;
    m_vertexBufferId = 0;
    m_vertexArrayId = 0;
    m_shaderId = 0;
}

void GLRainbowTriangle::NewFrame(float deltaT)
{
    if (!m_shaderId)
    {
        Init();
    }

    ImGui::SetNextWindowPos(ImVec2(300.f, 10.f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(m_name.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (ImGui::CollapsingHeader("Triangle Vertex Buffer"))
        {
            ImGui::InputFloat3("0", &m_bufData[0]);
            ImGui::InputFloat3("1", &m_bufData[3]);
            ImGui::InputFloat3("2", &m_bufData[6]);
        }
    }
    ImGui::End();
}

void GLRainbowTriangle::Render()
{
    glBindVertexArray(m_vertexArrayId);

    glUseProgram(m_shaderId);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
    // Fill the data every frame because it can change
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_bufData), m_bufData, GL_DYNAMIC_DRAW);
    // populate i_vertexPosition data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferId);
    // populate i_vertexColor data
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    // clear our VBO selection and GL state so future calls to GL don't pollute our data here.
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
