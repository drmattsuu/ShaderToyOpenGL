#include "GLRenderable.h"

#include "Util/ShaderUtils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include <string>

GLuint GLRenderable::LoadShader(const std::string& vertPath, const std::string& fragPath)
{
    return LoadShaderFile(vertPath, fragPath);
}

// clang-format off
const GLfloat GLHelloTriangle::s_bufData[] = {
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     0.0f,  1.0f, 0.0f,
};
// clang-format on

void GLHelloTriangle::Init()
{
    glGenVertexArrays(1, &m_vertexArrayId);
    glBindVertexArray(m_vertexArrayId);

    m_shaderId = LoadShader("resources/triangle.vert", "resources/triangle.frag");

    glGenBuffers(1, &m_vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(s_bufData), s_bufData, GL_STATIC_DRAW);
    m_colorUniformLocation = glGetUniformLocation(m_shaderId, "u_color");
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

void GLHelloTriangle::NewFrame()
{
    if (!m_shaderId)
    {
        Init();
    }

    if (ImGui::Begin(m_name.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::ColorPicker4("Triangle Color", m_color);
    }
    ImGui::End();
}

void GLHelloTriangle::Render()
{
    glBindVertexArray(m_vertexArrayId);

    glUseProgram(m_shaderId);

    glUniform4fv(m_colorUniformLocation, 1, m_color);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);

    // clear our VBO selection and GL state so future calls to GL don't pollute our data here.
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
