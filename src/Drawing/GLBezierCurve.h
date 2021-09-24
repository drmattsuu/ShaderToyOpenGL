#pragma once

#include "GLCamera.h"
#include "GLRenderable.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

class GLBezierCurve : public GLRenderable
{
public:
    GLBezierCurve(const int* displaySize);
    ~GLBezierCurve() { CleanGLResources(); }

    void Init() override;
    void CleanGLResources() override;
    void NewFrame(float deltaT) override;
    void Render() override;

private:
    float m_thickness = 8.f;

     GLuint m_nPoints = 4;
    // clang-format off
     std::vector<GLfloat> m_controlPoints = {
         0.9f,  0.9f,
         0.7f,  0.9f,
        -0.7f, -0.9f,
        -0.9f, -0.9f,
    };
     std::vector<GLfloat> m_colorPoints = {
        0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f,
    };
    // clang-format on

    GLuint m_vertexArrayId = 0;
    GLuint m_vertexBufferId = 0;
    GLuint m_colorBufferId = 0;
    GLuint m_shaderId = 0;

    const int* m_displaySize;
};
