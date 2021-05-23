#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <string>

class GLRenderable
{
public:
    GLRenderable(const std::string& name) : m_name(name) {}
    virtual ~GLRenderable() = default;

    virtual void Init() = 0;
    virtual void CleanGLResources() = 0;

    virtual void NewFrame() = 0;
    virtual void Render() = 0;

    bool ShouldRender() { return m_renderEnabled; }
    void SetShouldRender(bool shouldRender) { m_renderEnabled = shouldRender; }

    const std::string& GetName() { return m_name; }

protected:
    GLuint LoadShader(const std::string& vertPath, const std::string& fragPath);

protected:
    bool m_renderEnabled = true;
    std::string m_name;
};

using GLRenderablePtr = std::shared_ptr<GLRenderable>;
using GLRenderableWeakPtr = std::weak_ptr<GLRenderable>;

class GLHelloTriangle : public GLRenderable
{
public:
    GLHelloTriangle() : GLRenderable("HelloTriangle") {}
    ~GLHelloTriangle() { CleanGLResources(); }

    void Init() override;
    void CleanGLResources() override;

    void NewFrame() override;
    void Render() override;

private:
    GLuint m_vertexArrayId = 0;
    GLuint m_vertexBufferId = 0;
    GLuint m_shaderId = 0;
    GLint m_colorUniformLocation = -1;

    GLclampf m_color[4] = {0.3f, 0.3f, 1.f, 1.f};

    static const GLfloat s_bufData[];
};
