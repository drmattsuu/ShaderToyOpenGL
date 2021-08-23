#pragma once

#include "GLRenderable.h"

#include <map>
#include <string>

class GLShaderToyRenderable : public GLRenderable
{
public:
    GLShaderToyRenderable();
    ~GLShaderToyRenderable() { CleanGLResources(); }

    void Init() override;
    void CleanGLResources() override;

    void NewFrame(float deltaT) override;
    void Render() override;

private:
    int m_frame = 0;
    float m_runtime = 0.0f;

    GLuint m_currentShaderId = 0;

    std::map<std::string, GLuint> m_loadedShaders;
};
