#include "GLCubeRenderable.h"

#include "Util/ShaderUtils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <glm/ext.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace
{
// clang-format off
// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
static const GLfloat g_vBufData[] = { 
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
     1.0f, 1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
     1.0f,-1.0f, 1.0f
};

// Two UV coordinates for each vertex. They were created with Blender.
static const GLfloat g_uvBufData[] = { 
    0.000059f, 1.0f-0.000004f, 
    0.000103f, 1.0f-0.336048f, 
    0.335973f, 1.0f-0.335903f, 
    1.000023f, 1.0f-0.000013f, 
    0.667979f, 1.0f-0.335851f, 
    0.999958f, 1.0f-0.336064f, 
    0.667979f, 1.0f-0.335851f, 
    0.336024f, 1.0f-0.671877f, 
    0.667969f, 1.0f-0.671889f, 
    1.000023f, 1.0f-0.000013f, 
    0.668104f, 1.0f-0.000013f, 
    0.667979f, 1.0f-0.335851f, 
    0.000059f, 1.0f-0.000004f, 
    0.335973f, 1.0f-0.335903f, 
    0.336098f, 1.0f-0.000071f, 
    0.667979f, 1.0f-0.335851f, 
    0.335973f, 1.0f-0.335903f, 
    0.336024f, 1.0f-0.671877f, 
    1.000004f, 1.0f-0.671847f, 
    0.999958f, 1.0f-0.336064f, 
    0.667979f, 1.0f-0.335851f, 
    0.668104f, 1.0f-0.000013f, 
    0.335973f, 1.0f-0.335903f, 
    0.667979f, 1.0f-0.335851f, 
    0.335973f, 1.0f-0.335903f, 
    0.668104f, 1.0f-0.000013f, 
    0.336098f, 1.0f-0.000071f, 
    0.000103f, 1.0f-0.336048f, 
    0.000004f, 1.0f-0.671870f, 
    0.336024f, 1.0f-0.671877f, 
    0.000103f, 1.0f-0.336048f, 
    0.336024f, 1.0f-0.671877f, 
    0.335973f, 1.0f-0.335903f, 
    0.667969f, 1.0f-0.671889f, 
    1.000004f, 1.0f-0.671847f, 
    0.667979f, 1.0f-0.335851f
};
// clang-format on
}  // namespace

GLCubeRenderable::GLCubeRenderable(GLFWwindow* window)
    : GLRenderable("Cube"),
      m_view(glm::lookAt(glm::vec3(4.f, 3.f, 3.f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0))),
      m_model(1.f),
      m_window(window)
{
}

void GLCubeRenderable::Init()
{
    glGenVertexArrays(1, &m_vertexArrayId);
    glBindVertexArray(m_vertexArrayId);
    m_shaderId = LoadShader("resources/cube.vert", "resources/cube.frag");

    m_matrixUniformLocation = glGetUniformLocation(m_shaderId, "u_mvp");
    m_textureSamplerLocation = glGetUniformLocation(m_shaderId, "u_texSampler");

    glGenBuffers(1, &m_vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vBufData), g_vBufData, GL_STATIC_DRAW);
    glGenBuffers(1, &m_uvBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_uvBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uvBufData), g_uvBufData, GL_STATIC_DRAW);

    m_textureId = LoadTextureBMPFile("resources/cube.bmp");
}

void GLCubeRenderable::CleanGLResources()
{
    if (m_vertexBufferId)
        glDeleteBuffers(1, &m_vertexBufferId);
    if (m_uvBufferId)
        glDeleteBuffers(1, &m_uvBufferId);
    if (m_vertexArrayId)
        glDeleteVertexArrays(1, &m_vertexArrayId);
    if (m_shaderId)
        glDeleteProgram(m_shaderId);
    if (m_textureId)
        glDeleteTextures(1, &m_textureId);

    m_vertexBufferId = 0;
    m_uvBufferId = 0;
    m_vertexArrayId = 0;
    m_shaderId = 0;
    m_textureId = 0;
}

void GLCubeRenderable::NewFrame()
{
    if (!m_shaderId)
    {
        Init();
    }

    // todo : only update when the aspect needs updating / listen for resize events
    int displayW, displayH;
    glfwGetFramebufferSize(m_window, &displayW, &displayH);

    m_aspect = static_cast<float>(displayW) / static_cast<float>(displayH);
}

void GLCubeRenderable::Render()
{
    glm::mat4 projection = glm::perspective(glm::radians(m_fovDeg), m_aspect, 0.1f, 1000.f);
    // Model View Projection
    glm::mat4 mvp = projection * m_view * m_model;

    // Enable depth test
    bool prevEnableDepthTest = glIsEnabled(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LEQUAL);

    glBindVertexArray(m_vertexArrayId);

    glUseProgram(m_shaderId);

    glUniformMatrix4fv(m_matrixUniformLocation, 1, GL_FALSE, &mvp[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureId);

    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(m_textureSamplerLocation, 0);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_uvBufferId);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);  // 12*3 indices starting at 0 -> 12 triangles

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    if (prevEnableDepthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}
