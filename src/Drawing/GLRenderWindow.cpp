#include "GLRenderWindow.h"

#include "GLCubeRenderable.h"
#include "GLRenderable.h"

#include "ImguiImpl.h"
#include "imgui.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

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

    m_camera.SetPosition(glm::vec3(4.f, 3.f, 3.f));
    m_camera.LookAt(glm::vec3(0, 0, 0));
    float aspect = static_cast<float>(m_displaySize[0]) / static_cast<float>(m_displaySize[1]);
    m_camera.SetAspect(aspect);

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

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Appearing);
    float frameMs = io.DeltaTime * 1000.f;

    if (ImGui::Begin("##DebugInfoWindow", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        ImGui::Text("%.3f ms/frame (%.1f FPS)", frameMs, io.Framerate);
        ImGui::Checkbox("Show ImGui Demo", &g_ShowDemoWindow);
        ImGui::ColorEdit4("Clear Color", m_clearColor);
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Layers", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto renderable = m_renderables.begin();
            for (int i = 0; renderable != m_renderables.end(); ++renderable, ++i)
            {
                (*renderable)->SetShouldRender(false);
                ImGui::RadioButton((*renderable)->GetName().c_str(), &m_activeDemo, i);
            }
        }

        ImGui::Separator();
        ImGui::Text("Camera Controls");
        glm::vec3 camPos = m_camera.GetPosition();
        ImGui::InputFloat3("Position", &camPos[0]);
        if (camPos != m_camera.GetPosition())
        {
            m_camera.SetPosition(camPos);
        }

        static glm::vec3 lookAt(0.f);
        ImGui::InputFloat3("##LookAt", &lookAt[0]);
        ImGui::SameLine();
        if (ImGui::Button("LookAt"))
        {
            m_camera.LookAt(lookAt);
        }

        if (ImGui::CollapsingHeader("Camera Movement"))
        {
            if (ImGui::Button("L##mov"))
            {
                m_camera.MoveLocal(glm::vec3(0.5f, 0.0f, 0.0f));
            }
            ImGui::SameLine();
            if (ImGui::Button("R##mov"))
            {
                m_camera.MoveLocal(glm::vec3(-0.5f, 0.0f, 0.0f));
            }
            ImGui::SameLine();
            if (ImGui::Button("^##mov"))
            {
                m_camera.MoveLocal(glm::vec3(0.0f, 0.5f, 0.0f));
            }
            if (ImGui::Button("F##mov"))
            {
                m_camera.MoveLocal(glm::vec3(0.0f, 0.0f, 0.5f));
            }
            ImGui::SameLine();
            if (ImGui::Button("B##mov"))
            {
                m_camera.MoveLocal(glm::vec3(0.0f, 0.0f, -0.5f));
            }
            ImGui::SameLine();
            if (ImGui::Button("v##mov"))
            {
                m_camera.MoveLocal(glm::vec3(0.0f, -0.5f, 0.0f));
            }
        }
        if (ImGui::CollapsingHeader("Camera Rotation"))
        {
            if (ImGui::Button("<-##rot"))
            {
                m_camera.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 10.f);
            }
            ImGui::SameLine();
            if (ImGui::Button("->##rot"))
            {
                m_camera.Rotate(glm::vec3(0.0f, -1.0f, 0.0f), 10.f);
            }
            ImGui::SameLine();
            if (ImGui::Button("^##rot"))
            {
                m_camera.Rotate(glm::vec3(-1.0f, 0.0f, 0.0f), 10.f);
            }
            ImGui::SameLine();
            if (ImGui::Button("v##rot"))
            {
                m_camera.Rotate(glm::vec3(1.0f, 0.0f, 0.0f), 10.f);
            }
        }
    }
    ImGui::End();  // always call end

    if (m_activeDemo < m_renderables.size())
        m_renderables[m_activeDemo]->SetShouldRender(true);

    if (g_ShowDemoWindow)
        ImGui::ShowDemoWindow(&g_ShowDemoWindow);

    for (auto renderable : m_renderables)
    {
        if (renderable->ShouldRender())
            renderable->NewFrame(frameMs);
    }

    if (m_prevDisplaySize[0] != m_displaySize[0] || m_prevDisplaySize[1] != m_displaySize[1])
    {
        float aspect = static_cast<float>(m_displaySize[0]) / static_cast<float>(m_displaySize[1]);
        m_camera.SetAspect(aspect);
        m_prevDisplaySize[0] = m_displaySize[0];
        m_prevDisplaySize[1] = m_displaySize[1];
    }
}

void GLRenderWindow::Render()
{
    ImGui::Render();
    glfwGetFramebufferSize(m_window, &m_displaySize[0], &m_displaySize[1]);
    glViewport(0, 0, m_displaySize[0], m_displaySize[1]);
    glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Call Render functions
    for (auto renderable : m_renderables)
    {
        if (renderable->ShouldRender())
        {
            renderable->Render();
        }
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
    m_activeDemo = 0;
    m_renderables.erase(std::remove(m_renderables.begin(), m_renderables.end(), renderable), m_renderables.end());
}

void GLRenderWindow::AddAllRenderables()
{
    GLRenderablePtr helloTriangle(new GLHelloTriangle());
    AddRenderable(helloTriangle);

    GLRenderablePtr rainbowTriangle(new GLRainbowTriangle());
    AddRenderable(rainbowTriangle);

    GLRenderablePtr cube(new GLCubeRenderable(GetCamera()));
    AddRenderable(cube);
}
