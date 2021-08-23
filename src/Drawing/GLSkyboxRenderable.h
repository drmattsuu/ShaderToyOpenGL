#pragma once

#include "GLCamera.h"
#include "GLRenderable.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

class GLSkyboxRenderable : public GLRenderable
{
public:
    GLSkyboxRenderable(const GLCamera& camera);
    ~GLSkyboxRenderable() { CleanGLResources(); }

    void Init() override;
    void CleanGLResources() override;

    void NewFrame(float deltaT) override;
    void Render() override;

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_shaderId = 0;
    GLuint m_textureId = 0;

    GLint m_persUniformLocation = -1;
    GLint m_viewUniformLocation = -1;
    GLint m_cubeSamplerLocation = -1;

    const GLCamera& m_camera;
};