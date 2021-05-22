#pragma once

#include "GLRenderable.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <list>

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

private:
    bool m_contextCreated = false;
    GLFWwindow* m_window = nullptr;
    int m_displaySize[2] = {1280, 720};

    std::list<GLRenderablePtr> m_renderables;
};
