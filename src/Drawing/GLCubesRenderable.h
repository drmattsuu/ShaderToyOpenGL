#pragma once

#include "GLCamera.h"
#include "GLCubeRenderable.h"
#include "GLRenderable.h"

#include <vector>
#include <memory>

class GLCubesRenderable : public GLRenderable
{
public:
    GLCubesRenderable(const GLCamera& camera);
    ~GLCubesRenderable();

    void Init() override;
    void CleanGLResources() override;

    void NewFrame(float deltaT) override;
    void Render() override;

private:
    const GLCamera& m_camera;

    std::vector<GLCubeRenderablePtr> m_cubes;
};
