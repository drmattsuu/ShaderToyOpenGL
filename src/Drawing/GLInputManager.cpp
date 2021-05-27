#include "GLInputManager.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef _WIN32
#undef APIENTRY
#ifndef GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>  // for glfwGetWin32Window
#endif

//namespace
//{
//GLFWwindow* window = nullptr;
//
//// Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
//static GLFWmousebuttonfun g_PrevUserCallbackMousebutton = nullptr;
//static GLFWscrollfun g_PrevUserCallbackScroll = nullptr;
//static GLFWkeyfun g_PrevUserCallbackKey = nullptr;
//static GLFWcharfun g_PrevUserCallbackChar = nullptr;
//
//static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
//{
//    if (g_PrevUserCallbackMousebutton != nullptr)
//        g_PrevUserCallbackMousebutton(window, button, action, mods);
//
//    // if (action == GLFW_PRESS && button >= 0 && button < IM_ARRAYSIZE(g_MouseJustPressed))
//    //    g_MouseJustPressed[button] = true;
//}
//static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
//{
//    if (g_PrevUserCallbackScroll != nullptr)
//        g_PrevUserCallbackScroll(window, xoffset, yoffset);
//
//    // ImGuiIO& io = ImGui::GetIO();
//    // io.MouseWheelH += (float)xoffset;
//    // io.MouseWheel += (float)yoffset;
//}
//static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//    if (g_PrevUserCallbackKey != nullptr)
//        g_PrevUserCallbackKey(window, key, scancode, action, mods);
//
//    //    ImGuiIO& io = ImGui::GetIO();
//    //    if (action == GLFW_PRESS)
//    //        io.KeysDown[key] = true;
//    //    if (action == GLFW_RELEASE)
//    //        io.KeysDown[key] = false;
//    //
//    //    // Modifiers are not reliable across systems
//    //    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
//    //    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
//    //    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
//    //#ifdef _WIN32
//    //    io.KeySuper = false;
//    //#else
//    //    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
//    //#endif
//}
//static void CharCallback(GLFWwindow* window, unsigned int c)
//{
//    if (g_PrevUserCallbackChar != nullptr)
//        g_PrevUserCallbackChar(window, c);
//
//    // ImGuiIO& io = ImGui::GetIO();
//    // io.AddInputCharacter(c);
//}
//
//}  // namespace
//
//void InstallCallbacks(GLFWwindow* window)
//{
//    if (g_InstalledCallbacks && g_Window == window)
//        return;
//
//    g_Window = window;
//    g_InstalledCallbacks = true;
//    g_PrevUserCallbackMousebutton = glfwSetMouseButtonCallback(window, MouseButtonCallback);
//    g_PrevUserCallbackScroll = glfwSetScrollCallback(window, ScrollCallback);
//    g_PrevUserCallbackKey = glfwSetKeyCallback(window, KeyCallback);
//    g_PrevUserCallbackChar = glfwSetCharCallback(window, CharCallback);
//}
//
//void UninstallCallbacks()
//{
//}

void GLInputManager::InstallCallbacks(GLFWwindow* window)
{
}

void GLInputManager::UninstallCallbacks()
{
}
