#include "GLMeshRenderable.h"

#include "Util/FileUtils.h"
#include "Util/ShaderUtils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <assimp/Importer.hpp>
#include <glm/ext.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <vector>

GLMeshRenderable::GLMeshRenderable(const GLCamera& camera, const std::string& meshName,
                                   const std::string& meshType /*= ".obj"*/, GLuint shaderId /*= 0*/)
    : GLRenderable(meshName), m_camera(camera), m_shaderId(shaderId)
{
    m_meshFilename = "resources/" + meshName + meshType;
}

void GLMeshRenderable::Init()
{
    if (!m_shaderId)
    {
        m_shaderId = LoadShader("resources/shaders/model.vert", "resources/shaders/model.frag");
        m_ownsShader = true;
    }
    m_persUniformLocation = glGetUniformLocation(m_shaderId, "u_perspective");
    m_viewUniformLocation = glGetUniformLocation(m_shaderId, "u_view");
    m_modelUniformLocation = glGetUniformLocation(m_shaderId, "u_model");
    m_lightPosUniformLocation = glGetUniformLocation(m_shaderId, "u_lightPos");
    m_cameraPosUniformLocation = glGetUniformLocation(m_shaderId, "u_cameraPos");

    setupMesh();
}

void GLMeshRenderable::CleanGLResources()
{
    if (m_shaderId && m_ownsShader)
        glDeleteProgram(m_shaderId);

    m_shaderId = 0;

    if (m_model)
    {
        m_model.reset();
    }
}

void GLMeshRenderable::NewFrame(float deltaT)
{
    if (!m_model || !m_shaderId)
        Init();

    static bool lightFollowingCamera = false;

    ImGui::SetNextWindowPos(ImVec2(300.f, 10.f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(m_name.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::SliderFloat3("RPS", &m_rotSpeed[0], -1.f, 1.f, "%.2f");
        ImGui::InputFloat3("Light Pos", &m_lightPos[0], "%.1f");
        ImGui::Checkbox("Light Following Camera", &lightFollowingCamera);
        if (ImGui::Button("Reload Shader"))
        {
            if (m_shaderId)
                glDeleteProgram(m_shaderId);

            m_shaderId = LoadShader("resources/model.vert", "resources/model.frag");
            m_persUniformLocation = glGetUniformLocation(m_shaderId, "u_perspective");
            m_viewUniformLocation = glGetUniformLocation(m_shaderId, "u_view");
            m_modelUniformLocation = glGetUniformLocation(m_shaderId, "u_model");
            m_lightPosUniformLocation = glGetUniformLocation(m_shaderId, "u_lightPos");
            m_cameraPosUniformLocation = glGetUniformLocation(m_shaderId, "u_cameraPos");
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Matrix"))
        {
            // reset to identity matrix but maintain translation
            float x = m_modelMatrix[3][0];
            float y = m_modelMatrix[3][1];
            float z = m_modelMatrix[3][2];
            m_modelMatrix = glm::mat4(1.f);
            m_modelMatrix[3][0] = x;
            m_modelMatrix[3][1] = y;
            m_modelMatrix[3][2] = z;

            m_rotSpeed = glm::vec3(0.f);
        }
        if (ImGui::CollapsingHeader("Model Matrix"))
        {
            ImGui::InputFloat4("##row1", &m_modelMatrix[0][0]);
            ImGui::InputFloat4("##row2", &m_modelMatrix[1][0]);
            ImGui::InputFloat4("##row3", &m_modelMatrix[2][0]);
            ImGui::InputFloat4("##row4", &m_modelMatrix[3][0]);
        }
    }
    ImGui::End();

    if(lightFollowingCamera)
    {
        m_lightPos = m_camera.GetPosition() - m_camera.GetNormal() * 5.f;
    }

    if (m_rotSpeed != glm::vec3(0.f))
    {
        float spd = glm::length(m_rotSpeed);
        m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians((deltaT * 0.36f * spd)), m_rotSpeed);
    }
}

void GLMeshRenderable::Render()
{
    if (!m_model)
        return;

    // Enable depth test
    bool prevEnableDepthTest = glIsEnabled(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LEQUAL);

    glUseProgram(m_shaderId);

    glUniformMatrix4fv(m_persUniformLocation, 1, GL_FALSE, &m_camera.GetProjectionMatrix()[0][0]);
    glUniformMatrix4fv(m_viewUniformLocation, 1, GL_FALSE, &m_camera.GetViewMatrix()[0][0]);
    glUniformMatrix4fv(m_modelUniformLocation, 1, GL_FALSE, &m_modelMatrix[0][0]);
    glUniform3fv(m_lightPosUniformLocation, 1, &m_lightPos[0]);
    glUniform3fv(m_cameraPosUniformLocation, 1, &m_camera.GetPosition()[0]);

    m_model->Draw(m_shaderId);

    glUseProgram(0);

    if (prevEnableDepthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void GLMeshRenderable::SetPosition(const glm::vec3& worldPos)
{
    m_modelMatrix[3][0] = worldPos.x;
    m_modelMatrix[3][1] = worldPos.y;
    m_modelMatrix[3][2] = worldPos.z;
}

void GLMeshRenderable::Rotate(float degree, const glm::vec3& rotDir)
{
    m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(degree), rotDir);
}

void GLMeshRenderable::setupMesh()
{
    m_model = ModelPtr(new Model(m_meshFilename));
}
