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

protected:
    GLuint LoadShader(const std::string& vertPath, const std::string& fragPath);

private:
    GLint CompileShaderSrc(GLuint shaderId, const std::string& shaderSrc);
    GLint LinkShader(GLuint programId, GLuint vertShaderId, GLuint fragShaderId);

protected:
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

    static const GLfloat s_bufData[];
};
