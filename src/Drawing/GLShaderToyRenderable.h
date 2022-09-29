#pragma once

#include "GLRenderable.h"

#include <string>
#include <vector>
#include <memory>

struct ShaderToyShader
{
    std::string FilePath = "";
    GLuint ShaderId{0};
};

class GLShaderToyRenderable : public GLRenderable
{
public:
    GLShaderToyRenderable(const int* displaySize);
    ~GLShaderToyRenderable() { CleanGLResources(); }

    void Init() override;
    void CleanGLResources() override;

    void NewFrame(float deltaT) override;
    void Render() override;

private:
    GLuint ConstructShaderToyShader(std::string filePath);

    void ClearShaders();
    void LocateShaders();
    void ResetTime();

private:
    int m_frame = 0;
    float m_runTime = 0.0f;
    float m_deltaTime = 0.0f;

    const int* m_displaySize;

    bool m_paused = false;

    int m_currentShader = 0;
    std::vector<ShaderToyShader> m_shaders;

    GLuint m_quadVertexArrayObject = 0;
    GLuint m_quadVertexBufferObject = 0;
};

using GLShaderToyRenderablePtr = std::shared_ptr<GLShaderToyRenderable>;
