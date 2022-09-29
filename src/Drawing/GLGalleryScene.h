#pragma once

#include "GLCamera.h"
#include "GLCubeRenderable.h"
#include "GLFrameBuffersRenderable.h"
#include "GLRenderable.h"
#include "GLShaderToyRenderable.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>

class GLGalleryScene : public GLRenderable
{
public:
    GLGalleryScene(const GLCamera& camera);
    ~GLGalleryScene() { CleanGLResources(); }

    void Init() override;
    void CleanGLResources() override;
    void NewFrame(float deltaT) override;
    void Render() override;

    bool ShouldLoadImmediately() const override { return true; };

private:
    GLCubeRenderablePtr m_cube;
    GLFrameBuffersRenderablePtr m_frameBuffer;
    GLShaderToyRenderablePtr m_shaderToy;

    const GLCamera& m_camera;

    GLuint m_textureId = 0;

    int m_cubeTextureResolution[2] = {600, 600};
};
