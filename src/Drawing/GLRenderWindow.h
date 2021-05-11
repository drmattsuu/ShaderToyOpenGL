#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>


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

private:
    GLFWwindow* m_window = nullptr;
    int m_displaySize[2] = { 1280, 720 };
};
