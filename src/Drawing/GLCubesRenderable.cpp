#include "GLCubesRenderable.h"

#include "GLCamera.h"
#include "GLCubeRenderable.h"

#include <memory>
#include <vector>

GLCubesRenderable::GLCubesRenderable(const GLCamera& camera) : GLRenderable("Cubes"), m_camera(camera)
{
    m_cubes.push_back(GLCubeRenderablePtr(new GLCubeRenderable(camera)));
    m_cubes.push_back(GLCubeRenderablePtr(new GLCubeRenderable(camera)));
    m_cubes.push_back(GLCubeRenderablePtr(new GLCubeRenderable(camera)));

    m_cubes[1]->TranslateWorld(glm::vec3(-2.0f, 0.f, 0.f));
    m_cubes[2]->TranslateWorld(glm::vec3(2.0f, 0.f, 0.f));
}

GLCubesRenderable::~GLCubesRenderable()
{
    CleanGLResources();
    m_cubes.clear();
}

void GLCubesRenderable::Init()
{
    for (auto cube : m_cubes)
    {
        cube->Init();
    }
}

void GLCubesRenderable::CleanGLResources()
{
    for (auto cube : m_cubes)
    {
        cube->CleanGLResources();
    }
}

void GLCubesRenderable::NewFrame(float deltaT)
{
    for (auto cube : m_cubes)
    {
        cube->NewFrame(deltaT);
    }
}

void GLCubesRenderable::Render()
{
    for (auto cube : m_cubes)
    {
        cube->Render();
    }
}
