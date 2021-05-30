#pragma once

#include "GLCamera.h"
#include "GLInputManager.h"
#include "GLRenderable.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class GLCubeRenderable : public GLRenderable
{
public:
    GLCubeRenderable(const GLCamera& camera);
    ~GLCubeRenderable() { CleanGLResources(); }

    void Init() override;
    void CleanGLResources() override;

    void NewFrame(float deltaT) override;
    void Render() override;

    bool HandleKeyEvent(EventPtr event);

private:
    GLuint m_vertexBufferId = 0;
    GLuint m_uvBufferId = 0;
    GLuint m_vertexArrayId = 0;
    GLuint m_shaderId = 0;
    GLuint m_textureId = 0;

    GLint m_matrixUniformLocation = -1;
    GLint m_textureSamplerLocation = -1;

    glm::mat4 m_model;

    glm::vec3 m_rotSpeed{0.f};

    const GLCamera& m_camera;

    bool m_arrows[4]{false, false, false, false};
};
