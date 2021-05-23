#include "GLRenderWindow.h"

#include "GLRenderable.h"

#include "ImguiImpl.h"
#include "imgui.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

static bool g_ShowDemoWindow = false;

static void glfwErrorCallback(int error, const char* description)
{
    std::cout << "GLFW Error " << error << ": " << description << std::endl;
}

bool GLRenderWindow::GLInit()
{
    if (m_contextCreated)
    {
        return true;
    }

    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
    {
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(  // required on Mac OS
        GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(m_displaySize[0], m_displaySize[1], "ShaderToy OpenGL Demo", nullptr, nullptr);

    glfwMakeContextCurrent(m_window);
    // vsync
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // Don't save a ini with Imgui State
    io.IniFilename = nullptr;
    ImGui::StyleColorsDark();

    GLGui::Init(m_window, true);

    for (auto renderable : m_renderables)
    {
        renderable->Init();
    }
    m_contextCreated = true;
    return true;
}

void GLRenderWindow::GLCleanUp()
{
    m_renderables.clear();
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

    GLGui::NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Appearing);
    if (ImGui::Begin("##DebugInfoWindow", nullptr,
                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        float frameMs = 0.001f * ImGui::GetIO().Framerate;
        ImGui::Text("%.3f ms/frame (%.1f FPS)", frameMs, ImGui::GetIO().Framerate);
        ImGui::Checkbox("Show ImGui Demo", &g_ShowDemoWindow);
        ImGui::ColorEdit4("Clear Color", m_clearColor);
    }
    ImGui::End();  // always call end

    if (g_ShowDemoWindow)
        ImGui::ShowDemoWindow(&g_ShowDemoWindow);

    for (auto renderable : m_renderables)
    {
        renderable->NewFrame();
    }
}

void GLRenderWindow::Render()
{
    ImGui::Render();
    glfwGetFramebufferSize(m_window, &m_displaySize[0], &m_displaySize[1]);
    glViewport(0, 0, m_displaySize[0], m_displaySize[1]);
    glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    // Call Render functions
    for (auto renderable : m_renderables)
    {
        if (renderable->ShouldRender())
            renderable->Render();
    }

    GLGui::RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_window);
}

void GLRenderWindow::AddRenderable(GLRenderablePtr renderable)
{
    m_renderables.push_back(renderable);
    if (m_contextCreated)
    {
        renderable->Init();
    }
}

void GLRenderWindow::RemoveRenderable(GLRenderablePtr renderable)
{
    m_renderables.remove(renderable);
}
