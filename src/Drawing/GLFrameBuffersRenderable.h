#pragma once

#include "GLCamera.h"
#include "GLRenderable.h"

class GLFrameBuffersRenderable : public GLRenderable
{
public:
    GLFrameBuffersRenderable(const GLCamera& camera, const int* displaySize);
    ~GLFrameBuffersRenderable() { CleanGLResources(); }

    void Init() override;
    void CleanGLResources() override;

    void NewFrame(float deltaT) override;
    void Render() override;

    bool ShouldDrawSkybox() const override { return false; }

private:
    GLuint m_frameBufferObject = 0;
    GLuint m_renderBufferObject = 0;
    GLuint m_textureColorBuffer = 0;

    GLuint m_quadVertexArrayObject = 0;
    GLuint m_quadVertexBufferObject = 0;
    GLuint m_quadShaderId = 0;

    GLint m_screenTextureSamplerLocation = -1;
    
    GLRenderablePtr m_scene;
    GLRenderablePtr m_skybox;

    bool m_drawSkybox = true;
    const GLCamera& m_camera;
    const int* m_displaySize;
};
