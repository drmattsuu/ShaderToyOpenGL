#include "GLRenderable.h"

#include "Util/ShaderUtils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

    m_shaderProgramId = LoadShader("resources/triangle.vert", "resources/triangle.frag");

    glGenBuffers(1, &m_vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(s_bufData), s_bufData, GL_STATIC_DRAW);
}

void GLHelloTriangle::CleanGLResources()
{
    if (m_vertexBufferId)
        glDeleteBuffers(1, &m_vertexBufferId);
    if (m_vertexArrayId)
        glDeleteVertexArrays(1, &m_vertexArrayId);
    if (m_shaderProgramId)
        glDeleteProgram(m_shaderProgramId);

    m_vertexBufferId = 0;
    m_vertexArrayId = 0;
    m_shaderProgramId = 0;
}

void GLHelloTriangle::NewFrame()
{
    if (!m_vertexArrayId || !m_vertexBufferId || !m_shaderProgramId)
    {
        Init();
    }
}

void GLHelloTriangle::Render()
{
    glBindVertexArray(m_vertexArrayId);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
    // Normally we may need to re-populate our buffer-data here, but we can skip that step because the buffer-data is
    // constant and unchanging.
    // glBufferData(GL_ARRAY_BUFFER, sizeof(s_bufData), s_bufData, GL_STATIC_DRAW);

    glUseProgram(m_shaderProgramId);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);

    // clear our VBO selection and GL state so future calls to GL don't pollute our data here.
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
