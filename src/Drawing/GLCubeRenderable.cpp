#include "GLCubeRenderable.h"

#include "GLInputManager.h"
#include "Util/ShaderUtils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <glm/ext.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <vector>

namespace
{
// clang-format off
// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
static const std::vector<GLfloat> g_vBufData = { 
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

 //Two UV coordinates for each vertex. Generated from blender for the bmp, won't look right with a png
static const std::vector<GLfloat> g_uvBufData = { 
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

GLCubeRenderable::GLCubeRenderable(const GLCamera& camera) : GLRenderable("Cube"), m_camera(camera), m_model(1.f)
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
    glBufferData(GL_ARRAY_BUFFER, g_vBufData.size() * sizeof(GLfloat), &g_vBufData[0], GL_STATIC_DRAW);
    glGenBuffers(1, &m_uvBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_uvBufferId);
    // if using png comment this line and invert the UV buffer
    static const bool loadBMP = true;
    if (loadBMP)
    {
        glBufferData(GL_ARRAY_BUFFER, g_uvBufData.size() * sizeof(GLfloat), &g_uvBufData[0], GL_STATIC_DRAW);
        m_textureId = LoadTextureBMPFile("resources/cube2.bmp");
    }
    else
    {
        // Invert the UV buffer for PNG (only required because bmp is loaded upside down with my brain dead simple bmp
        // loader and directly using the same UV buffer results in "backwards" looking textures in PNG).
        // todo : finish this work by translating UV mappings 90 degrees to line up with BMP UV
        std::vector<GLfloat> uvBufDataInverted(g_uvBufData.size());
        auto it = uvBufDataInverted.begin();
        auto origIt = g_uvBufData.begin();
        for (; it != uvBufDataInverted.end(); ++it, ++origIt)
        {
            GLfloat& u = (*it);
            GLfloat& v = (*++it);
            const GLfloat& oldU = (*origIt);
            const GLfloat& oldV = (*++origIt);
            u = oldV;
            v = oldU;
        }
        glBufferData(GL_ARRAY_BUFFER, uvBufDataInverted.size() * sizeof(GLfloat), &uvBufDataInverted[0],
                     GL_STATIC_DRAW);
        m_textureId = LoadTexturePNGFile("resources/cube.png");
    }

    SubscribeEvent(EventType::Key)->bind(this, &GLCubeRenderable::HandleKeyEvent);
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

void GLCubeRenderable::NewFrame(float deltaT)
{
    if (!m_shaderId)
    {
        Init();
    }

    ImGui::SetNextWindowPos(ImVec2(300.f, 10.f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(m_name.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::SliderFloat3("RPS", &m_rotSpeed[0], -1.f, 1.f, "%.2f");
        if (ImGui::Button("Reload Shader"))
        {
            if (m_shaderId)
                glDeleteProgram(m_shaderId);

            m_shaderId = LoadShader("resources/cube.vert", "resources/cube.frag");

            m_matrixUniformLocation = glGetUniformLocation(m_shaderId, "u_mvp");
            m_textureSamplerLocation = glGetUniformLocation(m_shaderId, "u_texSampler");
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Rotation"))
        {
            // reset to identity matrix
            m_model = glm::mat4(1.f);
            m_rotSpeed = glm::vec3(0.f);
        }
        if (ImGui::CollapsingHeader("Texture"))
            ImGui::Image((ImTextureID)(intptr_t)(m_textureId), ImVec2(256.f, 256.f));
    }

    if (m_arrows[0])
    {
        m_rotSpeed += glm::vec3(0.001f * deltaT, 0.0f, 0.0f);
    }
    if (m_arrows[1])
    {
        m_rotSpeed -= glm::vec3(0.001f * deltaT, 0.0f, 0.0f);
    }
    if (m_arrows[2])
    {
        m_rotSpeed += glm::vec3(0.0f, 0.001f * deltaT, 0.0f);
    }
    if (m_arrows[3])
    {
        m_rotSpeed -= glm::vec3(0.0f, 0.001f * deltaT, 0.0f);
    }
    ImGui::End();
    if (m_rotSpeed != glm::vec3(0.f))
    {
        float spd = glm::length(m_rotSpeed);
        m_model = glm::rotate(m_model, glm::radians((deltaT * 0.36f * spd)), m_rotSpeed);
    }
}

void GLCubeRenderable::Render()
{
    // Model View Projection
    glm::mat4 mvp = m_camera.GetProjectionMatrix() * m_camera.GetViewMatrix() * m_model;

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

bool GLCubeRenderable::HandleKeyEvent(EventPtr event)
{
    if (!ShouldRender())
    {
        // Don't eat other events
        return false;
    }
    auto buttonEvent = dynamic_cast<ButtonEvent*>(event.get());
    if (buttonEvent)
    {
        bool pressed = buttonEvent->Action != GLFW_RELEASE;
        int key = buttonEvent->Button;
        switch (key)
        {
            case GLFW_KEY_UP:
                m_arrows[0] = pressed;
                break;
            case GLFW_KEY_DOWN:
                m_arrows[1] = pressed;
                break;
            case GLFW_KEY_LEFT:
                m_arrows[2] = pressed;
                break;
            case GLFW_KEY_RIGHT:
                m_arrows[3] = pressed;
                break;
            default:
                return false;
        }
        return true;
    }
    return false;
}
