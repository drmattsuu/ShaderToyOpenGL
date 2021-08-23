#pragma once

#include "GLCamera.h"
#include "GLRenderable.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>

class GLRenderWindow
{
public:
    GLRenderWindow() = default;
    ~GLRenderWindow() = default;

    bool GLInit();
    void GLCleanUp();

    bool GetShouldClose();

    void NewFrame();
    void Render();

    void AddRenderable(GLRenderablePtr renderable);
    void RemoveRenderable(GLRenderablePtr renderable);

    void AddAllRenderables();

    GLFWwindow* GetWindow() { return m_window; }

    const GLCamera& GetCamera() const { return m_camera; }

private:
    bool m_contextCreated = false;
    bool m_renderSkybox = false;
    int m_displaySize[2] = {1280, 720};
    int m_prevDisplaySize[2] = {1280, 720};
    int m_activeDemo = 0;

    GLFWwindow* m_window = nullptr;
    GLclampf m_clearColor[4] = {0.45f, 0.55f, 0.60f, 1.00f};

    std::vector<GLRenderablePtr> m_renderables;
    GLRenderablePtr m_skybox;

    GLCamera m_camera;
};
