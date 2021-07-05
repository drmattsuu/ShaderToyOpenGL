#include "GLInputManager.h"

#include "Util/FastDelegate.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>

#include <algorithm>
#include <deque>
#include <map>
#include <memory>

namespace
{
static std::shared_ptr<GLInputManager> g_InputMan;

// Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
static GLFWmousebuttonfun g_PrevUserCallbackMousebutton = nullptr;
static GLFWscrollfun g_PrevUserCallbackScroll = nullptr;
static GLFWkeyfun g_PrevUserCallbackKey = nullptr;
static GLFWcharfun g_PrevUserCallbackChar = nullptr;

static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    GLInputManager::GetInputManager()->HandleMouse_cb(window, button, action, mods);
}
static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    GLInputManager::GetInputManager()->HandleScroll_cb(window, xoffset, yoffset);
}
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    GLInputManager::GetInputManager()->HandleKey_cb(window, key, scancode, action, mods);
}
static void CharCallback(GLFWwindow* window, unsigned int c)
{
    GLInputManager::GetInputManager()->HandleChar_cb(window, c);
}

}  // namespace

std::shared_ptr<GLInputManager> GLInputManager::GetInputManager()
{
    if (nullptr == g_InputMan.get())
    {
        g_InputMan = std::shared_ptr<GLInputManager>(new GLInputManager());
    }
    return g_InputMan;
}

void GLInputManager::InstallCallbacks(GLFWwindow* window)
{
    if (m_installedCallbacks)
        return;

    m_installedCallbacks = true;
    g_PrevUserCallbackMousebutton = glfwSetMouseButtonCallback(window, MouseButtonCallback);
    g_PrevUserCallbackScroll = glfwSetScrollCallback(window, ScrollCallback);
    g_PrevUserCallbackKey = glfwSetKeyCallback(window, KeyCallback);
    g_PrevUserCallbackChar = glfwSetCharCallback(window, CharCallback);
}

void GLInputManager::UninstallCallbacks(GLFWwindow* window)
{
    if (!m_installedCallbacks)
        return;

    m_installedCallbacks = false;
    glfwSetMouseButtonCallback(window, g_PrevUserCallbackMousebutton);
    glfwSetScrollCallback(window, g_PrevUserCallbackScroll);
    glfwSetKeyCallback(window, g_PrevUserCallbackKey);
    glfwSetCharCallback(window, g_PrevUserCallbackChar);
}

void GLInputManager::Update(float deltaT, GLFWwindow* window)
{
    const bool focused = glfwGetWindowAttrib(window, GLFW_FOCUSED) != 0;
    if (focused)
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        if (m_cursorPos[0] != x && m_cursorPos[1] != y)
        {
            double deltaX = x - m_cursorPos[0];
            double deltaY = y - m_cursorPos[1];
            m_cursorPos[0] = x;
            m_cursorPos[1] = y;
            EventPtr event(new MouseXYEvent(EventType::MouseMove, deltaX, deltaY));
            SendEvent(event);
        }
    }
}

EventDelegatePtr GLInputManager::SubscribeEvent(EventType e, bool pushToFront)
{
    // todo : thread safety
    EventDelegatePtr newDelegate(new EventDelegate);
    if (pushToFront)
        m_subscribers[e].push_front(newDelegate);
    else
        m_subscribers[e].push_back(newDelegate);
    return newDelegate;
}

void GLInputManager::UnsubscribeEvent(EventType e, EventDelegatePtr d)
{
    // todo : thread safety
    std::deque<EventDelegatePtr>& delegates = m_subscribers[e];
    delegates.erase(std::remove(delegates.begin(), delegates.end(), d), delegates.end());
}

void GLInputManager::HandleMouse_cb(GLFWwindow* window, int button, int action, int mods)
{
    if (g_PrevUserCallbackMousebutton != nullptr)
        g_PrevUserCallbackMousebutton(window, button, action, mods);

    EventPtr event(new ButtonEvent(EventType::MouseButton, button, action, mods));
    SendEvent(event);
}

void GLInputManager::HandleScroll_cb(GLFWwindow* window, double xoffset, double yoffset)
{
    if (g_PrevUserCallbackScroll != nullptr)
        g_PrevUserCallbackScroll(window, xoffset, yoffset);
    EventPtr event(new MouseXYEvent(EventType::MouseScroll, xoffset, yoffset));
    SendEvent(event);
}

void GLInputManager::HandleKey_cb(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (g_PrevUserCallbackKey != nullptr)
        g_PrevUserCallbackKey(window, key, scancode, action, mods);

    EventPtr event(new ButtonEvent(EventType::Key, key, action, mods, scancode));
    SendEvent(event);
}

void GLInputManager::HandleChar_cb(GLFWwindow* window, unsigned int c)
{
    if (g_PrevUserCallbackChar != nullptr)
        g_PrevUserCallbackChar(window, c);

    EventPtr event(new CharEvent(EventType::Char, c));
    SendEvent(event);
}

void GLInputManager::SendEvent(EventPtr event)
{
    for (EventDelegatePtr delegate : m_subscribers[event->Type])
    {
        if ((*delegate)(event))
        {
            // event has been handled in an exclusive way, break from the loop
            break;
        }
    }
}
