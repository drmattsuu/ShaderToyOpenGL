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
     GLuint m_nPoints = 4;
    // clang-format off
     std::vector<GLfloat> m_controlPoints = {
         0.9f,  0.9f,
         0.0f,  0.9f,
         0.0f, -0.9f,
        -0.9f, -0.9f,
    };
     std::vector<GLfloat> m_colorPoints = {
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
    };
    // clang-format on

    float m_thickness = 20.f;
    int m_segments = 30;
    float m_dashSize = 20.f;
    float m_gapSize = 20.f;


    GLuint m_vertexArrayId = 0;
    GLuint m_vertexBufferId = 0;
    GLuint m_colorBufferId = 0;
    GLuint m_bezierShaderId = 0;
    GLuint m_shortCurveShaderId = 0;
    GLuint m_lineShaderId = 0;
    GLuint m_dashedLineShaderId = 0;

    enum DrawMode
    {
        Bezier = 0,
        ShortCurve = 1,
        DashedLine = 2,
        MAX
    };
    DrawMode m_drawMode = Bezier;

    const int* m_displaySize;
};
