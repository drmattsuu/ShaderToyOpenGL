#include "GLSkyboxRenderable.h"

#include "Util/ShaderUtils.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{
// clang-format off
static const std::vector<GLfloat> g_vBufData = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
// clang-format on
}  // namespace

GLSkyboxRenderable::GLSkyboxRenderable(const GLCamera& camera) : GLRenderable("Skybox"), m_camera(camera)
{
}

void GLSkyboxRenderable::Init()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, g_vBufData.size() * sizeof(GLfloat), &g_vBufData[0], GL_STATIC_DRAW);

    m_shaderId = LoadShader("Resources/shaders/skybox.vert", "Resources/shaders/skybox.frag");
    m_persUniformLocation = glGetUniformLocation(m_shaderId, "u_perspective");
    m_viewUniformLocation = glGetUniformLocation(m_shaderId, "u_view");
    m_cubeSamplerLocation = glGetUniformLocation(m_shaderId, "texture_skybox");

    std::vector<std::string> faces{"resources/skybox/right.jpg", "resources/skybox/left.jpg",
                                   "resources/skybox/top.jpg",   "resources/skybox/bottom.jpg",
                                   "resources/skybox/front.jpg", "resources/skybox/back.jpg"};

    m_textureId = LoadCubeMapSTB(faces);
}

void GLSkyboxRenderable::CleanGLResources()
{
    if (m_vbo)
        glDeleteBuffers(1, &m_vbo);
    if (m_vao)
        glDeleteVertexArrays(1, &m_vao);
    if (m_textureId)
        glDeleteTextures(1, &m_textureId);
    if (m_shaderId)
        glDeleteProgram(m_shaderId);

    m_vao = m_vbo = m_textureId = m_shaderId = 0;
}

void GLSkyboxRenderable::NewFrame(float deltaT)
{
    if (m_shaderId == 0)
    {
        Init();
    }
}

void GLSkyboxRenderable::Render()
{
    // Enable depth test
    bool prevEnableDepthTest = glIsEnabled(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(m_vao);

    glUseProgram(m_shaderId);

    glUniformMatrix4fv(m_persUniformLocation, 1, GL_FALSE, &m_camera.GetProjectionMatrix()[0][0]);

    glm::mat4 view = glm::mat4(glm::mat3(m_camera.GetViewMatrix()));

    glUniformMatrix4fv(m_viewUniformLocation, 1, GL_FALSE, &view[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureId);

    glUniform1i(m_cubeSamplerLocation, 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);  // 12*3 indices starting at 0 -> 12 triangles

    glDisableVertexAttribArray(0);

    if (prevEnableDepthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}
