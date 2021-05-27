#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>

class GLInputManager
{
public:

    ~GLInputManager() = default;

    void InstallCallbacks(GLFWwindow* window);
    void UninstallCallbacks();

private:        
    GLInputManager() = default;

private:
    //static std::shared_ptr<GLInput>

    GLFWwindow* m_window = nullptr;
};
