#include "GLRenderWindow.h"

#include "GLCubeRenderable.h"
#include "GLCubesRenderable.h"
#include "GLFrameBuffersRenderable.h"
#include "GLInputManager.h"
#include "GLMeshRenderable.h"
#include "GLRenderable.h"
#include "GLShaderToyRenderable.h"
#include "GLSkyboxRenderable.h"

#include "ImguiImpl.h"
#include "imgui.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

static bool g_ShowDemoWindow = false;
static bool g_cullBackfaces = true;
static int g_cullMode = GL_BACK;

static void glfwErrorCallback(int error, const char* description)
{
    std::cout << "GLFW Error " << error << ": " << description << std::endl;
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
                            const char* message, const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            std::cout << "Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            std::cout << "Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            std::cout << "Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            std::cout << "Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            std::cout << "Source: Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            std::cout << "Source: Other";
            break;
    }
    std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            std::cout << "Type: Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cout << "Type: Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cout << "Type: Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            std::cout << "Type: Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cout << "Type: Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            std::cout << "Type: Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            std::cout << "Type: Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            std::cout << "Type: Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            std::cout << "Type: Other";
            break;
    }
    std::cout << std::endl;

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            std::cout << "Severity: high";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cout << "Severity: medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            std::cout << "Severity: low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::cout << "Severity: notification";
            break;
    }
    std::cout << std::endl;
    std::cout << std::endl;
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
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

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

    GLInputManager::GetInputManager()->InstallCallbacks(m_window);

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

    int maxAttrs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttrs);
    std::cout << "Maximum vertex attributes supported: " << maxAttrs << std::endl;

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    return true;
}

void GLRenderWindow::GLCleanUp()
{
    GLInputManager::GetInputManager()->UninstallCallbacks(m_window);
    m_renderables.clear();
    GLGui::Shutdown();
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool GLRenderWindow::GetShouldClose()
{
    return glfwWindowShouldClose(m_window);
}

void GLRenderWindow::NewFrame()
{
    m_renderSkybox = false;
    glfwPollEvents();

    GLGui::NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    float frameMs = io.DeltaTime * 1000.f;

    GLInputManager::GetInputManager()->Update(frameMs, m_window);
    m_camera.Update(frameMs);

    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Appearing);
    if (ImGui::Begin("##DebugInfoWindow", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        ImGui::Text("%.3f ms/frame (%.1f FPS)", frameMs, io.Framerate);
        if (ImGui::Button("Cleanup GL Resources"))
        {
            for (auto renderable : m_renderables)
            {
                renderable->CleanGLResources();
            }
        }
        ImGui::Checkbox("Show ImGui Demo", &g_ShowDemoWindow);
        ImGui::ColorEdit4("Clear Color", m_clearColor);
        ImGui::Checkbox("Cull Backfaces", &g_cullBackfaces);
        if (g_cullBackfaces)
        {
            const int glFaceBits[] = {GL_BACK, GL_FRONT, GL_FRONT_AND_BACK};
            const char* glFaceBitLabels[] = {"GL_BACK", "GL_FRONT", "GL_FRONT_AND_BACK"};
            static int currentItemIndex = 0;
            const char* currentLabel = glFaceBitLabels[currentItemIndex];
            if (ImGui::BeginCombo("CullFace", currentLabel))
            {
                for (int i = 0; i < IM_ARRAYSIZE(glFaceBits); ++i)
                {
                    const bool isSelected = currentItemIndex == i;
                    if (ImGui::Selectable(glFaceBitLabels[i], isSelected))
                        currentItemIndex = i;

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            g_cullMode = glFaceBits[currentItemIndex];
        }
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
    }
    ImGui::End();  // always call end

    if (m_activeDemo < m_renderables.size())
        m_renderables[m_activeDemo]->SetShouldRender(true);

    if (g_ShowDemoWindow)
        ImGui::ShowDemoWindow(&g_ShowDemoWindow);

    for (auto renderable : m_renderables)
    {
        if (renderable->ShouldRender())
        {
            m_renderSkybox = renderable->ShouldDrawSkybox() || m_renderSkybox;
            renderable->NewFrame(frameMs);
        }
    }

    if (m_renderSkybox)
        m_skybox->NewFrame(frameMs);

    if (m_prevDisplaySize[0] != m_displaySize[0] || m_prevDisplaySize[1] != m_displaySize[1])
    {
        if (m_displaySize[0] != 0.f && m_displaySize[1] != 0.f)
        {
            float aspect = static_cast<float>(m_displaySize[0]) / static_cast<float>(m_displaySize[1]);
            m_camera.SetAspect(aspect);
            m_prevDisplaySize[0] = m_displaySize[0];
            m_prevDisplaySize[1] = m_displaySize[1];
        }
    }
}

void GLRenderWindow::Render()
{
    ImGui::Render();
    glfwGetFramebufferSize(m_window, &m_displaySize[0], &m_displaySize[1]);
    glViewport(0, 0, m_displaySize[0], m_displaySize[1]);
    glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    bool cullingBackfaces = glIsEnabled(GL_CULL_FACE);
    if (g_cullBackfaces)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(g_cullMode);
    }

    if (m_renderSkybox)
    {
        m_skybox->Render();
    }

    // Call Render functions
    for (auto renderable : m_renderables)
    {
        if (renderable->ShouldRender())
        {
            renderable->Render();
        }
    }

    if (cullingBackfaces)
    {
        glEnable(GL_CULL_FACE);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }

    GLGui::RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_window);
}

void GLRenderWindow::AddRenderable(GLRenderablePtr renderable)
{
    m_renderables.push_back(renderable);
    if (m_contextCreated && renderable->ShouldLoadImmediately())
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
    m_skybox = GLRenderablePtr(new GLSkyboxRenderable(GetCamera()));

    AddRenderable(GLRenderablePtr(new GLShaderToyRenderable(m_displaySize)));

    GLRenderablePtr helloTriangle(new GLHelloTriangle());
    AddRenderable(helloTriangle);

    GLRenderablePtr rainbowTriangle(new GLRainbowTriangle());
    AddRenderable(rainbowTriangle);

    GLRenderablePtr cube(new GLCubeRenderable(GetCamera()));
    AddRenderable(cube);

    GLRenderablePtr cubes(new GLCubesRenderable(GetCamera()));
    AddRenderable(cubes);

    AddRenderable(GLRenderablePtr(new GLFrameBuffersRenderable(GetCamera(), m_displaySize)));

    AddRenderable(GLRenderablePtr(new GLMeshRenderable(GetCamera(), "Monkey")));

    AddRenderable(GLRenderablePtr(new GLMeshRenderable(GetCamera(), "planet/planet")));

    AddRenderable(GLRenderablePtr(new GLMeshRenderable(GetCamera(), "rock/rock")));

    // AddRenderable(GLRenderablePtr(new GLMeshRenderable(GetCamera(), "vampire/dancing_vampire", ".dae")));

    AddRenderable(GLRenderablePtr(new GLMeshRenderable(GetCamera(), "nanosuit/nanosuit")));

    AddRenderable(GLRenderablePtr(new GLMeshRenderable(GetCamera(), "cyborg/cyborg")));

    AddRenderable(GLRenderablePtr(new GLMeshRenderable(GetCamera(), "backpack/backpack")));

    // AddRenderable(GLRenderablePtr(new GLMeshRenderable(GetCamera(), "radar")));
}
