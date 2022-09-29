#pragma once

#include "GLCamera.h"
#include "GLInputManager.h"
#include "GLRenderable.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>

class GLCubeRenderable : public GLRenderable
{
public:
    GLCubeRenderable(const GLCamera& camera, GLuint textureId = 0);
    ~GLCubeRenderable() { CleanGLResources(); }

    void Init() override;
    void CleanGLResources() override;

    void NewFrame(float deltaT) override;
    void Render() override;

    bool ShouldDrawSkybox() const override { return true; }

    bool HandleKeyEvent(EventPtr event);

    void TranslateWorld(const glm::vec3& by);

private:
    GLuint m_vertexBufferId = 0;
    GLuint m_uvBufferId = 0;
    GLuint m_normalBufferId = 0;
    GLuint m_vertexArrayId = 0;
    GLuint m_shaderId = 0;
    GLuint m_textureId = 0;
    
    GLint m_persUniformLocation = -1;
    GLint m_viewUniformLocation = -1;
    GLint m_modelUniformLocation = -1;
    GLint m_lightPosUniformLocation = -1;
    GLint m_textureSamplerLocation = -1;

    glm::mat4 m_model{1.f};

    glm::vec3 m_rotSpeed{0.f};

    glm::vec3 m_lightPos{5.f, 10.f, 5.f};

    const GLCamera& m_camera;

    bool m_arrows[4]{false, false, false, false};
    bool m_ownsTexture{false};
};

using GLCubeRenderablePtr = std::shared_ptr<GLCubeRenderable>;
