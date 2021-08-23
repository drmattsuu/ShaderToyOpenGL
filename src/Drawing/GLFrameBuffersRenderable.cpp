#include "GLFrameBuffersRenderable.h"
#include "GLCubeRenderable.h"
#include "GLMeshRenderable.h"
#include "GLSkyboxRenderable.h"

#include "imgui.h"

#include <iostream>
#include <string>

namespace
{
// clang-format off
//const static float kQuadVertices[] = {
//    // positions   // texCoords
//    -0.3f,  1.0f,  0.0f, 1.0f,
//    -0.3f,  0.7f,  0.0f, 0.0f,
//     0.3f,  0.7f,  1.0f, 0.0f,
//
//    -0.3f,  1.0f,  0.0f, 1.0f,
//     0.3f,  0.7f,  1.0f, 0.0f,
//     0.3f,  1.0f,  1.0f, 1.0f
//};

const static float kQuadVertices[] = {
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};
// clang-format on
}  // namespace

GLFrameBuffersRenderable::GLFrameBuffersRenderable(const GLCamera& camera, const int* displaySize)
    : GLRenderable("FrameBuffers"),
      m_camera(camera),
      m_displaySize(displaySize),
      m_scene(new GLMeshRenderable(camera, "cyborg/cyborg")),
      m_skybox(new GLSkyboxRenderable(camera))
{
}

void GLFrameBuffersRenderable::Init()
{
    m_skybox->Init();
    m_scene->Init();

    glGenFramebuffers(1, &m_frameBufferObject);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObject);

    glGenTextures(1, &m_textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_displaySize[0], m_displaySize[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureColorBuffer, 0);

    glGenRenderbuffers(1, &m_renderBufferObject);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderBufferObject);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_displaySize[0], m_displaySize[1]);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderBufferObject);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenVertexArrays(1, &m_quadVertexArrayObject);
    glGenBuffers(1, &m_quadVertexBufferObject);
    glBindVertexArray(m_quadVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kQuadVertices), &kQuadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    m_quadShaderId = LoadShader("resources/shaders/framebuffer.vert", "resources/shaders/framebuffer.frag");
    m_screenTextureSamplerLocation = glGetUniformLocation(m_quadShaderId, "screenTexture");
}

void GLFrameBuffersRenderable::CleanGLResources()
{
    if (m_quadShaderId)
        glDeleteProgram(m_quadShaderId);

    if (m_quadVertexBufferObject)
        glDeleteBuffers(1, &m_quadVertexBufferObject);
    if (m_quadVertexArrayObject)
        glDeleteVertexArrays(1, &m_quadVertexArrayObject);

    if (m_textureColorBuffer)
        glDeleteTextures(1, &m_textureColorBuffer);
    if (m_renderBufferObject)
        glDeleteRenderbuffers(1, &m_renderBufferObject);
    if (m_frameBufferObject)
        glDeleteFramebuffers(1, &m_frameBufferObject);

    m_frameBufferObject = m_renderBufferObject = m_textureColorBuffer = 0;
    m_quadShaderId = m_quadVertexArrayObject = m_quadVertexBufferObject = 0;

    m_scene->CleanGLResources();
    m_skybox->CleanGLResources();
}

void GLFrameBuffersRenderable::NewFrame(float deltaT)
{
    if (!m_frameBufferObject)
        Init();

    if (ImGui::Begin("FrameBuffer"))
    {
        if (ImGui::Button("Reload GL Resources"))
        {
            CleanGLResources();
            Init();
        }
        ImGui::Checkbox("Draw Skybox", &m_drawSkybox);
        ImGui::Image((ImTextureID)(intptr_t)(m_textureColorBuffer),
                     ImVec2(m_displaySize[0] * 0.3f, m_displaySize[1] * 0.3f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
    }
    ImGui::End();

    m_skybox->NewFrame(deltaT);
    m_scene->NewFrame(deltaT);
}

void GLFrameBuffersRenderable::Render()
{
    bool prevEnableDepthTest = glIsEnabled(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);

    // m_scene->Render();

    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObject);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (m_drawSkybox)
        m_skybox->Render();

    m_scene->Render();

    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_DEPTH_TEST);

    glUseProgram(m_quadShaderId);

    glUniform1i(m_screenTextureSamplerLocation, 0);

    glBindVertexArray(m_quadVertexArrayObject);
    glBindTexture(GL_TEXTURE_2D, m_textureColorBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glUseProgram(0);

    if (prevEnableDepthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}
