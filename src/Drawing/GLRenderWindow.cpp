#include "GLRenderWindow.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

static void glfwErrorCallback(int error, const char* description)
{
    std::cout << "GLFW Error " << error << ": " << description << std::endl;
}

bool GLRenderWindow::GLInit()
{
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
    {
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    m_window = glfwCreateWindow(m_displaySize[0], m_displaySize[1], "ShaderToy OpenGL Demo", nullptr, nullptr);

    glfwMakeContextCurrent(m_window);
    // vsync
    glfwSwapInterval(1);

    if(glewInit() != GLEW_OK)
    {
        return false;
    }

    return true;
}

void GLRenderWindow::GLCleanUp()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool GLRenderWindow::GetShouldClose()
{
    return glfwWindowShouldClose(m_window);
}

void GLRenderWindow::NewFrame()
{
    glfwPollEvents();

    // Create/Update Device Objects
}

void GLRenderWindow::Render()
{
    // Call Render functions

    glfwGetFramebufferSize(m_window, &m_displaySize[0], &m_displaySize[1]);
    glViewport(0, 0, m_displaySize[0], m_displaySize[1]);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(m_window);
}

