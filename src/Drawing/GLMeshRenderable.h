#pragma once

#include "GLCamera.h"
#include "GLRenderable.h"
#include "Model/Model.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>
#include <string>

class GLMeshRenderable : public GLRenderable
{
public:
    GLMeshRenderable(const GLCamera& camera, const std::string& meshName, const std::string& meshType = ".obj");
    ~GLMeshRenderable() { CleanGLResources(); }

    void Init() override;
    void CleanGLResources() override;

    void NewFrame(float deltaT) override;
    void Render() override;

    void SetPosition(const glm::vec3& worldPos);
    void Rotate(float degree, const glm::vec3& rotDir);

private:
    GLuint m_shaderId = 0;

    GLint m_persUniformLocation = -1;
    GLint m_viewUniformLocation = -1;
    GLint m_modelUniformLocation = -1;
    GLint m_lightPosUniformLocation = -1;
    GLint m_cameraPosUniformLocation = -1;

    glm::mat4 m_modelMatrix{1.f};
    glm::vec3 m_lightPos{5.f, 10.f, 5.f};
    glm::vec3 m_rotSpeed{0.f};

    const GLCamera& m_camera;

    std::string m_meshFilename;

    ModelPtr m_model;
};
