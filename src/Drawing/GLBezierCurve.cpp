#include "GLBezierCurve.h"

#include "Util/ShaderUtils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

GLBezierCurve::GLBezierCurve(const int* displaySize) : GLRenderable("Bezier"), m_displaySize(displaySize)
{
}

void GLBezierCurve::Init()
{
    m_shaderId = LoadGeomShaderFile("resources/shaders/bezier.vert", "resources/shaders/bezier.frag",
                                    "resources/shaders/bezier2.geom");

    glGenVertexArrays(1, &m_vertexArrayId);
    glBindVertexArray(m_vertexArrayId);
    glGenBuffers(1, &m_vertexBufferId);
    glGenBuffers(1, &m_colorBufferId);
}

void GLBezierCurve::CleanGLResources()
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

void GLBezierCurve::NewFrame(float deltaT)
{
    if (!m_shaderId)
    {
        Init();
    }

    ImGui::SetNextWindowPos(ImVec2(300.f, 10.f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(m_name.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (ImGui::Button("Reload"))
        {
            CleanGLResources();
            Init();
        }

        ImGui::InputFloat("Thickness", &m_thickness);

        if (ImGui::CollapsingHeader("Control Points"))
        {
            ImGui::InputFloat2("0##vtx", &m_controlPoints[0]);
            ImGui::InputFloat2("1##vtx", &m_controlPoints[2]);
            ImGui::InputFloat2("2##vtx", &m_controlPoints[4]);
            ImGui::InputFloat2("3##vtx", &m_controlPoints[6]);
        }
        if (ImGui::CollapsingHeader("Color Points"))
        {
            ImGui::InputFloat3("0##color", &m_colorPoints[0]);
            ImGui::InputFloat3("1##color", &m_colorPoints[3]);
            ImGui::InputFloat3("2##color", &m_colorPoints[6]);
            ImGui::InputFloat3("3##color", &m_colorPoints[9]);
        }
    }

    ImGui::End();
}

void GLBezierCurve::Render()
{
    glUseProgram(m_shaderId);

    glUniform1f(0, m_thickness);
    glm::vec2 viewport(m_displaySize[0], m_displaySize[1]);
    glUniform2fv(1, 1, &viewport[0]);
    //glUniform1f(2, 0.1f);

    glBindVertexArray(m_vertexArrayId);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, m_controlPoints.size() * sizeof(GLfloat), &m_controlPoints[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferId);
    glBufferData(GL_ARRAY_BUFFER, m_colorPoints.size() * sizeof(GLfloat), &m_colorPoints[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_LINES_ADJACENCY_EXT, 0, m_nPoints);

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
