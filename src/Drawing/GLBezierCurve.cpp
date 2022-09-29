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
    m_bezierShaderId = LoadGeomShaderFile("resources/shaders/bezier.vert", "resources/shaders/bezier.frag",
                                          "resources/shaders/bezier2.geom");

    m_shortCurveShaderId = LoadGeomShaderFile("resources/shaders/bezier.vert", "resources/shaders/bezier.frag",
                                              "resources/shaders/bezToShortCurve.geom");

    m_lineShaderId = LoadGeomShaderFile("resources/shaders/bezier.vert", "resources/shaders/bezier.frag",
                                        "resources/shaders/bezToLine.geom");

    m_dashedLineShaderId = LoadGeomShaderFile("resources/shaders/bezToLineDashed.vert", "resources/shaders/bezToLineDashed.frag",
                                              "resources/shaders/bezToLineDashed.geom");

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
    if (m_bezierShaderId)
        glDeleteProgram(m_bezierShaderId);
    if (m_shortCurveShaderId)
        glDeleteProgram(m_shortCurveShaderId);
    if (m_lineShaderId)
        glDeleteProgram(m_lineShaderId);
    if (m_dashedLineShaderId)
        glDeleteProgram(m_dashedLineShaderId);

    m_colorBufferId = 0;
    m_vertexBufferId = 0;
    m_vertexArrayId = 0;
    m_bezierShaderId = 0;
    m_shortCurveShaderId = 0;
    m_lineShaderId = 0;
    m_dashedLineShaderId = 0;
}

void GLBezierCurve::NewFrame(float deltaT)
{
    if (!m_bezierShaderId || !m_shortCurveShaderId || !m_lineShaderId || !m_dashedLineShaderId)
    {
        CleanGLResources();
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

        const char* drawModeLabels[] = {"Bezier", "ShortCurve", "DashedLine"};
        if (ImGui::BeginCombo("Draw Mode", drawModeLabels[m_drawMode]))
        {
            for (int i = 0; i < DrawMode::MAX; ++i)
            {
                const bool isSelected = m_drawMode == i;
                if (ImGui::Selectable(drawModeLabels[i], isSelected))
                {
                    m_drawMode = static_cast<DrawMode>(i);
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::SliderFloat("Thickness", &m_thickness, 1.f, 100.f, "%.0f", 1.0f);

        if (m_drawMode == Bezier || m_drawMode == ShortCurve)
        {
            ImGui::SliderInt("Segments", &m_segments, 2, 30);
        }
        if (m_drawMode == DashedLine)
        {
            ImGui::SliderFloat("DashSize", &m_dashSize, 2.f, 100, "%.0f", 1.0f);
            ImGui::SliderFloat("GapSize", &m_gapSize, 2.f, 100, "%.0f", 1.0f);
        }

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

    int n = 1;
    for (int i = 0; i < 8; i+=2)
    {
        //if (i == 4)
        //{
        //    ++n;
        //    continue;
        //}
        GLfloat& xVal = m_controlPoints[i];
        GLfloat& yVal = m_controlPoints[i + 1];

        float relativeX = (xVal + 1.0f) * 0.5f;
        float relativeY = (yVal + 1.0f) * 0.5f;

        float positionX = relativeX * m_displaySize[0];
        float positionY = (1.f - relativeY) * m_displaySize[1];

        ImVec2 pos(positionX, positionY);
        ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
        const auto flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar;
        const std::string id = "##ControlPoint" + std::to_string(i);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.7f, 0.7f, 0.7f, 0.25f));
        if (ImGui::Begin(id.c_str(), nullptr, flags))
        {
            std::string controlPointText = std::to_string(n++);
            ImGui::Text(controlPointText.c_str());

            auto newPos = ImGui::GetWindowPos();

            float newRelX = newPos.x / static_cast<float>(m_displaySize[0]);
            float newRelY = 1.f - (newPos.y / static_cast<float>(m_displaySize[1]));

            float cPosX = (newRelX - 0.5f) * 2.f;
            float cPosY = (newRelY - 0.5f) * 2.f;

            xVal = cPosX;
            yVal = cPosY;
        }
        ImGui::End();
        ImGui::PopStyleColor();
    }

    //m_controlPoints[4] = m_controlPoints[6];
    //m_controlPoints[5] = m_controlPoints[7];
}

void GLBezierCurve::Render()
{
    if (m_segments > 2)
    {
        if (m_drawMode == Bezier)
        {
            glUseProgram(m_bezierShaderId);
            glUniform1i(3, m_segments);
        }
        else if (m_drawMode == ShortCurve)
        {
            glm::vec2 start{m_controlPoints[0], m_controlPoints[1]};
            glm::vec2 end{m_controlPoints[6], m_controlPoints[7]};
            glm::vec2 c1{m_controlPoints[2], m_controlPoints[3]};
            glm::vec2 c2{m_controlPoints[4], m_controlPoints[5]};

            float startRadius = glm::length(c1 - start);
            float endRadius = glm::length(c2 - end);

            bool drawAsLine = (start.y == end.y) || (startRadius == 0.0f && endRadius == 0.0f);
            if (drawAsLine)
            {
                glUseProgram(m_lineShaderId);
            }
            else
            {
                glUseProgram(m_shortCurveShaderId);
                glUniform1i(3, m_segments);
            }
        }
        else if (m_drawMode == DashedLine)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

            glUseProgram(m_dashedLineShaderId);
            glUniform1f(2, m_dashSize);
            glUniform1f(3, m_gapSize);
        }
    }
    else
    {
        glUseProgram(m_lineShaderId);
    }

    glUniform1f(0, m_thickness);
    glm::vec2 viewport(m_displaySize[0], m_displaySize[1]);
    glUniform2fv(1, 1, &viewport[0]);

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
