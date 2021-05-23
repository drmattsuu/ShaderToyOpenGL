#pragma once

#include "GLRenderable.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class GLCubeRenderable : public GLRenderable
{
public:
    GLCubeRenderable(GLFWwindow* window);
    ~GLCubeRenderable() { CleanGLResources(); }

    void Init() override;
    void CleanGLResources() override;

    void NewFrame() override;
    void Render() override;

private:
    GLuint m_vertexBufferId = 0;
    GLuint m_uvBufferId = 0;
    GLuint m_vertexArrayId = 0;
    GLuint m_shaderId = 0;
    GLuint m_textureId = 0;

    GLint m_matrixUniformLocation = -1;
    GLint m_textureSamplerLocation = -1;
    
    float m_fovDeg = 45.f;
    float m_aspect = 1.33f;
    glm::mat4 m_view;
    glm::mat4 m_model;

    GLFWwindow* m_window;
};
