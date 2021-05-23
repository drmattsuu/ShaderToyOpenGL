#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <string>

class GLRenderable
{
public:
    GLRenderable() = default;
    virtual ~GLRenderable() = default;

    virtual void Init() = 0;
    virtual void CleanGLResources() = 0;

    virtual void NewFrame() = 0;
    virtual void Render() = 0;

    bool ShouldRender() { return m_renderEnabled; }

protected:
    GLuint LoadShader(const std::string& vertPath, const std::string& fragPath);

protected:
    bool m_renderEnabled = true;
};

using GLRenderablePtr = std::shared_ptr<GLRenderable>;
using GLRenderableWeakPtr = std::weak_ptr<GLRenderable>;

class GLHelloTriangle : public GLRenderable
{
public:
    GLHelloTriangle() : GLRenderable(){};
    ~GLHelloTriangle() { CleanGLResources(); }

    void Init() override;
    void CleanGLResources() override;

    void NewFrame() override;
    void Render() override;

private:
    GLuint m_vertexArrayId = 0;
    GLuint m_vertexBufferId = 0;
    GLuint m_shaderProgramId = 0;
    GLint m_colorUniformLocation = -1;

    GLclampf m_color[4] = {0.3f, 0.3f, 1.f, 1.f};

    static const GLfloat s_bufData[];
};
