#include "GLGalleryScene.h"

#include "Util/ShaderUtils.h"

GLGalleryScene::GLGalleryScene(const GLCamera& camera) : GLRenderable("GalleryScene"), m_camera(camera)
{
}

void GLGalleryScene::Init()
{
    if (!m_frameBuffer)
    {
        m_frameBuffer = std::make_shared<GLFrameBuffersRenderable>(m_camera, m_cubeTextureResolution);
    }
    m_frameBuffer->Init();

    if (!m_shaderToy)
    {
        m_shaderToy = std::make_shared<GLShaderToyRenderable>(m_cubeTextureResolution);
    }
    m_shaderToy->Init();

    if (!m_cube)
    {
        m_cube = std::make_shared<GLCubeRenderable>(m_camera, m_frameBuffer->GetTextureColorBuffer());
    }
    m_cube->Init();
}

void GLGalleryScene::CleanGLResources()
{
    if (m_cube)
    {
        m_cube->CleanGLResources();
    }
    if (m_shaderToy)
    {
        m_shaderToy->CleanGLResources();
    }
    if (m_frameBuffer)
    {
        m_frameBuffer->CleanGLResources();
    }
}

void GLGalleryScene::NewFrame(float deltaT)
{
    if (!m_frameBuffer || !m_frameBuffer->GetTextureColorBuffer())
        Init();

    // don't call NewFrame on Framebuffer to avoid reserving memory for its internal demo scene.
    // m_frameBuffer->NewFrame(deltaT);
    m_shaderToy->NewFrame(deltaT);
    m_cube->NewFrame(deltaT);
}

void GLGalleryScene::Render()
{
    m_frameBuffer->StartFrameBufferRender();
    m_shaderToy->Render();
    m_frameBuffer->EndFrameBufferRender();

    m_cube->Render();
}
