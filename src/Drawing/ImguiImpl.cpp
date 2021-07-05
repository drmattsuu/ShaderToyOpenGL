#include "ImguiImpl.h"

#include "imgui.h"

#include "Util/ShaderUtils.h"

#include <stdint.h>
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef _WIN32
#undef APIENTRY
#ifndef GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>  // for glfwGetWin32Window
#endif

namespace
{
static GLFWwindow* g_Window = nullptr;
static double g_Time = 0.0;
static bool g_MouseJustPressed[ImGuiMouseButton_COUNT] = {};
static GLFWcursor* g_MouseCursors[ImGuiMouseCursor_COUNT] = {};
static bool g_InstalledCallbacks = false;

// Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
static GLFWmousebuttonfun g_PrevUserCallbackMousebutton = nullptr;
static GLFWscrollfun g_PrevUserCallbackScroll = nullptr;
static GLFWkeyfun g_PrevUserCallbackKey = nullptr;
static GLFWcharfun g_PrevUserCallbackChar = nullptr;

// OpenGL Data
static GLuint g_FontTexture = 0;
static GLuint g_ShaderHandle = 0;
static GLint g_AttribLocationTex = 0;
static GLint g_AttribLocationProjMtx = 0;
static GLuint g_AttribLocationVtxPos = 0;
static GLuint g_AttribLocationVtxUV = 0;
static GLuint g_AttribLocationVtxColor = 0;
static unsigned int g_VboHandle = 0;
static unsigned int g_ElementsHandle = 0;

static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (g_PrevUserCallbackMousebutton != nullptr)
        g_PrevUserCallbackMousebutton(window, button, action, mods);

    if (action == GLFW_PRESS && button >= 0 && button < IM_ARRAYSIZE(g_MouseJustPressed))
        g_MouseJustPressed[button] = true;
}
static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (g_PrevUserCallbackScroll != nullptr)
        g_PrevUserCallbackScroll(window, xoffset, yoffset);

    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheelH += (float)xoffset;
    io.MouseWheel += (float)yoffset;
}
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (g_PrevUserCallbackKey != nullptr)
        g_PrevUserCallbackKey(window, key, scancode, action, mods);

    ImGuiIO& io = ImGui::GetIO();
    if (action == GLFW_PRESS)
        io.KeysDown[key] = true;
    if (action == GLFW_RELEASE)
        io.KeysDown[key] = false;

    // Modifiers are not reliable across systems
    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
#ifdef _WIN32
    io.KeySuper = false;
#else
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
#endif
}
static void CharCallback(GLFWwindow* window, unsigned int c)
{
    if (g_PrevUserCallbackChar != nullptr)
        g_PrevUserCallbackChar(window, c);

    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(c);
}

static const char* GetClipboardText(void* userData)
{
    GLFWwindow* window = static_cast<GLFWwindow*>(userData);
    return glfwGetClipboardString(window);
}
static void SetClipboardText(void* userData, const char* text)
{
    GLFWwindow* window = static_cast<GLFWwindow*>(userData);
    glfwSetClipboardString(window, text);
}

static void UpdateMousePosAndButtons()
{
    // Update buttons
    ImGuiIO& io = ImGui::GetIO();
    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
    {
        // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events
        // that are shorter than 1 frame.
        io.MouseDown[i] = g_MouseJustPressed[i] || glfwGetMouseButton(g_Window, i) != 0;
        g_MouseJustPressed[i] = false;
    }

    // Update mouse position
    const ImVec2 prevMousePos = io.MousePos;
    io.MousePos.x = -FLT_MAX;
    io.MousePos.y = -FLT_MAX;
    const bool focused = glfwGetWindowAttrib(g_Window, GLFW_FOCUSED) != 0;
    if (focused)
    {
        if (io.WantSetMousePos)
        {
            glfwSetCursorPos(g_Window, static_cast<double>(prevMousePos.x), static_cast<double>(prevMousePos.y));
        }
        else
        {
            double x, y;
            glfwGetCursorPos(g_Window, &x, &y);
            io.MousePos.x = static_cast<float>(x);
            io.MousePos.y = static_cast<float>(y);
        }
    }
}
static void UpdateMouseCursor()
{
    ImGuiIO& io = ImGui::GetIO();
    if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) ||
        glfwGetInputMode(g_Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        return;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    else
    {
        // Show OS mouse cursor
        // FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
        glfwSetCursor(g_Window, g_MouseCursors[imgui_cursor] ? g_MouseCursors[imgui_cursor]
                                                             : g_MouseCursors[ImGuiMouseCursor_Arrow]);
        glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

static void SetupRenderState(ImDrawData* drawData, int fbWidth, int fbHeight, GLuint vbo)
{
    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_SCISSOR_TEST);
    glDisable(GL_PRIMITIVE_RESTART);
#ifdef GL_POLYGON_MODE
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

    // Support for GL 4.5 rarely used glClipControl(GL_UPPER_LEFT)
#if defined(GL_CLIP_ORIGIN) && !defined(__APPLE__)
    bool clip_origin_lower_left = true;
    GLenum current_clip_origin = 0;
    glGetIntegerv(GL_CLIP_ORIGIN, (GLint*)&current_clip_origin);
    if (current_clip_origin == GL_UPPER_LEFT)
        clip_origin_lower_left = false;
#endif

    // Setup viewport, orthographic projection matrix
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to
    // draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
    glViewport(0, 0, (GLsizei)fbWidth, (GLsizei)fbHeight);
    float L = drawData->DisplayPos.x;
    float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
    float T = drawData->DisplayPos.y;
    float B = drawData->DisplayPos.y + drawData->DisplaySize.y;
#if defined(GL_CLIP_ORIGIN) && !defined(__APPLE__)
    if (!clip_origin_lower_left)
    {
        float tmp = T;
        T = B;
        B = tmp;
    }  // Swap top and bottom if origin is upper left
#endif

    float orthRL = 2.0f / (R - L);
    float orthTB = 2.0f / (T - B);
    float orthX = (R + L) / (L - R);
    float orthY = (T + B) / (B - T);
    const float ortho_projection[4][4] = {
        {orthRL, 0.0f, 0.0f, 0.0f},
        {0.0f, orthTB, 0.0f, 0.0f},
        {0.0f, 0.0f, -1.0f, 0.0f},
        {orthX, orthY, 0.0f, 1.0f},
    };
    glUseProgram(g_ShaderHandle);
    glUniform1i(g_AttribLocationTex, 0);
    glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);

    // We use combined texture/sampler state. Applications using GL 3.3 may set that otherwise.
    glBindSampler(0, 0);

    glBindVertexArray(vbo);

    // Bind vertex/index buffers and setup attributes for ImDrawVert
    glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
    glEnableVertexAttribArray(g_AttribLocationVtxPos);
    glEnableVertexAttribArray(g_AttribLocationVtxUV);
    glEnableVertexAttribArray(g_AttribLocationVtxColor);
    glVertexAttribPointer(g_AttribLocationVtxPos, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
                          (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
    glVertexAttribPointer(g_AttribLocationVtxUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
                          (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
    glVertexAttribPointer(g_AttribLocationVtxColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert),
                          (GLvoid*)IM_OFFSETOF(ImDrawVert, col));
}

struct GLGuiState
{
    GLenum prevActiveTexture;
    GLuint prevCurrentProgram;
    GLuint prevTextureBinding2D;
    GLuint prevSamplerBinding;
    GLuint prevArrayBuf;
    GLuint prevVBO;
    GLint prevPolyMode[2];
    GLint prevViewport[4];
    GLint prevScissorBox[4];
    GLenum prevBlendSrcRGB;
    GLenum prevBlendDstRGB;
    GLenum prevBlendSrcAlpha;
    GLenum prevBlendDstAlpha;
    GLenum prevBlendEqRgb;
    GLenum prevBlendEqAlpha;
    GLboolean prevEnableBlend;
    GLboolean prevEnableCullFace;
    GLboolean prevEnableDepthTest;
    GLboolean prevEnableStencilTest;
    GLboolean prevEnableScissorTest;
    GLboolean prevEnablePrimitiveRestart;
};

static GLGuiState BackupGLState()
{
    GLGuiState result;
    glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&result.prevActiveTexture);
    glActiveTexture(GL_TEXTURE0);
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&result.prevCurrentProgram);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&result.prevTextureBinding2D);
    glGetIntegerv(GL_SAMPLER_BINDING, (GLint*)&result.prevSamplerBinding);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*)&result.prevArrayBuf);

    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&result.prevVBO);

#ifdef GL_POLYGON_MODE
    glGetIntegerv(GL_POLYGON_MODE, result.prevPolyMode);
#endif
    glGetIntegerv(GL_VIEWPORT, result.prevViewport);
    glGetIntegerv(GL_SCISSOR_BOX, result.prevScissorBox);
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&result.prevBlendSrcRGB);
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&result.prevBlendDstRGB);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&result.prevBlendSrcAlpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&result.prevBlendDstAlpha);
    glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&result.prevBlendEqRgb);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&result.prevBlendEqAlpha);
    result.prevEnableBlend = glIsEnabled(GL_BLEND);
    result.prevEnableCullFace = glIsEnabled(GL_CULL_FACE);
    result.prevEnableDepthTest = glIsEnabled(GL_DEPTH_TEST);
    result.prevEnableStencilTest = glIsEnabled(GL_STENCIL_TEST);
    result.prevEnableScissorTest = glIsEnabled(GL_SCISSOR_TEST);
    result.prevEnablePrimitiveRestart = glIsEnabled(GL_PRIMITIVE_RESTART);

    return result;
}

static void RestoreGLState(const GLGuiState& glState)
{
    // Restore modified GL state
    glUseProgram(glState.prevCurrentProgram);
    glBindTexture(GL_TEXTURE_2D, glState.prevTextureBinding2D);
    glBindSampler(0, glState.prevSamplerBinding);
    glActiveTexture(glState.prevActiveTexture);
    glBindVertexArray(glState.prevVBO);
    glBindBuffer(GL_ARRAY_BUFFER, glState.prevArrayBuf);
    glBlendEquationSeparate(glState.prevBlendEqRgb, glState.prevBlendEqAlpha);
    glBlendFuncSeparate(glState.prevBlendSrcRGB, glState.prevBlendDstRGB, glState.prevBlendSrcAlpha,
                        glState.prevBlendDstAlpha);
    if (glState.prevEnableBlend)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
    if (glState.prevEnableCullFace)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
    if (glState.prevEnableDepthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    if (glState.prevEnableStencilTest)
        glEnable(GL_STENCIL_TEST);
    else
        glDisable(GL_STENCIL_TEST);
    if (glState.prevEnableScissorTest)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
    if (glState.prevEnablePrimitiveRestart)
        glEnable(GL_PRIMITIVE_RESTART);
    else
        glDisable(GL_PRIMITIVE_RESTART);

#ifdef GL_POLYGON_MODE
    glPolygonMode(GL_FRONT_AND_BACK, (GLenum)glState.prevPolyMode[0]);
#endif
    glViewport(glState.prevViewport[0], glState.prevViewport[1], (GLsizei)glState.prevViewport[2],
               (GLsizei)glState.prevViewport[3]);
    glScissor(glState.prevScissorBox[0], glState.prevScissorBox[1], (GLsizei)glState.prevScissorBox[2],
              (GLsizei)glState.prevScissorBox[3]);
}

}  // namespace

bool GLGui::Init(GLFWwindow* window, bool installCallbacks)
{
    g_Window = window;
    g_Time = 0.0;

    // Setup backend capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;  // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |=
        ImGuiBackendFlags_HasSetMousePos;  // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "imgui_impl_glfw";

    // Keyboard mapping. Dear ImGui will use those indices to peek into the io.KeysDown[] array.
    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

    io.SetClipboardTextFn = SetClipboardText;
    io.GetClipboardTextFn = GetClipboardText;
    io.ClipboardUserData = g_Window;
#if defined(_WIN32)
    io.ImeWindowHandle = (void*)glfwGetWin32Window(g_Window);
#endif

    // Create mouse cursors
    // (By design, on X11 cursors are user configurable and some cursors may be missing. When a cursor doesn't exist,
    // GLFW will emit an error which will often be printed by the app, so we temporarily disable error reporting.
    // Missing cursors will return nullptr and our _UpdateMouseCursor() function will use the Arrow cursor instead.)
    GLFWerrorfun prev_error_callback = glfwSetErrorCallback(nullptr);
    g_MouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    glfwSetErrorCallback(prev_error_callback);

    // Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
    g_PrevUserCallbackMousebutton = nullptr;
    g_PrevUserCallbackScroll = nullptr;
    g_PrevUserCallbackKey = nullptr;
    g_PrevUserCallbackChar = nullptr;
    if (installCallbacks)
    {
        g_InstalledCallbacks = true;
        g_PrevUserCallbackMousebutton = glfwSetMouseButtonCallback(window, MouseButtonCallback);
        g_PrevUserCallbackScroll = glfwSetScrollCallback(window, ScrollCallback);
        g_PrevUserCallbackKey = glfwSetKeyCallback(window, KeyCallback);
        g_PrevUserCallbackChar = glfwSetCharCallback(window, CharCallback);
    }

    // Setup backend capabilities flags
    io.BackendRendererName = "imgui_impl_opengl4";
    // We can honor the ImDrawCmd::VtxOffset field allowing for large meshes.
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    return true;
}

void GLGui::Shutdown()
{
    if (g_InstalledCallbacks)
    {
        glfwSetMouseButtonCallback(g_Window, g_PrevUserCallbackMousebutton);
        glfwSetScrollCallback(g_Window, g_PrevUserCallbackScroll);
        glfwSetKeyCallback(g_Window, g_PrevUserCallbackKey);
        glfwSetCharCallback(g_Window, g_PrevUserCallbackChar);
        g_InstalledCallbacks = false;
    }

    for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
    {
        glfwDestroyCursor(g_MouseCursors[cursor_n]);
        g_MouseCursors[cursor_n] = nullptr;
    }

    GLGui::DestroyDeviceObjects();
}

void GLGui::NewFrame()
{
    if (!g_ShaderHandle)
        GLGui::CreateDeviceObjects();

    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt() &&
              "Font atlas not built! It is generally built by the renderer backend. Missing call to renderer "
              "_NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    int displayW, displayH;
    glfwGetWindowSize(g_Window, &w, &h);
    glfwGetFramebufferSize(g_Window, &displayW, &displayH);
    io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
    if (w > 0 && h > 0)
        io.DisplayFramebufferScale = ImVec2(static_cast<float>(displayW / w), static_cast<float>(displayH / h));

    // Setup time step
    double currentTime = glfwGetTime();
    io.DeltaTime = g_Time > 0.0 ? static_cast<float>(currentTime - g_Time) : 0.0166f;
    g_Time = currentTime;

    UpdateMousePosAndButtons();
    UpdateMouseCursor();
}

void GLGui::RenderDrawData(ImDrawData* drawData)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer
    // coordinates)
    int fbWidth = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
    int fbHeight = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);
    if (fbWidth <= 0 || fbHeight <= 0)
        return;

    GLGuiState glState = BackupGLState();

    // Setup desired GL state
    // Recreate the VAO every time (this is to easily allow multiple GL contexts to be rendered to. VAO are not shared
    // among GL contexts) The renderer would actually work without any VAO bound, but then our VertexAttrib calls would
    // overwrite the default one currently bound.
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    SetupRenderState(drawData, fbWidth, fbHeight, vao);

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clipOff = drawData->DisplayPos;          // (0,0) unless using multi-viewports
    ImVec2 clipScale = drawData->FramebufferScale;  // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    for (int n = 0; n < drawData->CmdListsCount; n++)
    {
        const ImDrawList* cmdList = drawData->CmdLists[n];

        // Upload vertex/index buffers
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmdList->VtxBuffer.Size * (int)sizeof(ImDrawVert),
                     (const GLvoid*)cmdList->VtxBuffer.Data, GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmdList->IdxBuffer.Size * (int)sizeof(ImDrawIdx),
                     (const GLvoid*)cmdList->IdxBuffer.Data, GL_STREAM_DRAW);

        for (int iCmd = 0; iCmd < cmdList->CmdBuffer.Size; iCmd++)
        {
            const ImDrawCmd* pCmd = &cmdList->CmdBuffer[iCmd];
            if (pCmd->UserCallback != NULL)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer
                // to reset render state.)
                if (pCmd->UserCallback == ImDrawCallback_ResetRenderState)
                    SetupRenderState(drawData, fbWidth, fbHeight, vao);
                else
                    pCmd->UserCallback(cmdList, pCmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec4 clipRect;
                clipRect.x = (pCmd->ClipRect.x - clipOff.x) * clipScale.x;
                clipRect.y = (pCmd->ClipRect.y - clipOff.y) * clipScale.y;
                clipRect.z = (pCmd->ClipRect.z - clipOff.x) * clipScale.x;
                clipRect.w = (pCmd->ClipRect.w - clipOff.y) * clipScale.y;

                if (clipRect.x < fbWidth && clipRect.y < fbHeight && clipRect.z >= 0.0f && clipRect.w >= 0.0f)
                {
                    // Apply scissor/clipping rectangle
                    int cX = static_cast<int>(clipRect.x);
                    int cY = static_cast<int>(fbHeight - clipRect.w);
                    int cZ = static_cast<int>(clipRect.z - clipRect.x);
                    int cW = static_cast<int>(clipRect.w - clipRect.y);
                    glScissor(cX, cY, cZ, cW);

                    // Bind texture, Draw
                    glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pCmd->TextureId);
                    glDrawElementsBaseVertex(GL_TRIANGLES, (GLsizei)pCmd->ElemCount,
                                             sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
                                             (void*)(intptr_t)(pCmd->IdxOffset * sizeof(ImDrawIdx)),
                                             (GLint)pCmd->VtxOffset);
                }
            }
        }
    }

    RestoreGLState(glState);

    // Destroy the temporary VAO
    glDeleteVertexArrays(1, &vao);
}

bool GLGui::CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    // Load as RGBA 32-bit (75% of the memory is wasted, but default font is so small)
    // because it is more likely to be compatible with user's existing shaders. If your
    // ImTextureId represent a higher-level concept than just a GL texture id, consider
    // calling GetTexDataAsAlpha8() instead to save on GPU memory.
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Upload texture to graphics system
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &g_FontTexture);
    glBindTexture(GL_TEXTURE_2D, g_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#ifdef GL_UNPACK_ROW_LENGTH
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->SetTexID((ImTextureID)(intptr_t)g_FontTexture);

    // Restore state
    glBindTexture(GL_TEXTURE_2D, last_texture);

    return true;
}

void GLGui::DestroyFontsTexture()
{
    if (g_FontTexture)
    {
        ImGuiIO& io = ImGui::GetIO();
        glDeleteTextures(1, &g_FontTexture);
        io.Fonts->SetTexID(0);
        g_FontTexture = 0;
    }
}

bool GLGui::CreateDeviceObjects()
{
    // Backup GL state
    GLint prevTexture, prevArrayBuf;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prevArrayBuf);
    GLint prevVertexArray;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prevVertexArray);

    g_ShaderHandle = LoadShaderFile("resources/shaders/imgui.vert", "resources/shaders/imgui.frag");

    g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
    g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");
    g_AttribLocationVtxPos = (GLuint)glGetAttribLocation(g_ShaderHandle, "Position");
    g_AttribLocationVtxUV = (GLuint)glGetAttribLocation(g_ShaderHandle, "UV");
    g_AttribLocationVtxColor = (GLuint)glGetAttribLocation(g_ShaderHandle, "Color");

    // Create buffers
    glGenBuffers(1, &g_VboHandle);
    glGenBuffers(1, &g_ElementsHandle);

    GLGui::CreateFontsTexture();

    // Restore modified GL state
    glBindTexture(GL_TEXTURE_2D, prevTexture);
    glBindBuffer(GL_ARRAY_BUFFER, prevArrayBuf);
    glBindVertexArray(prevVertexArray);

    return true;
}

void GLGui::DestroyDeviceObjects()
{
    if (g_VboHandle)
    {
        glDeleteBuffers(1, &g_VboHandle);
    }
    if (g_ElementsHandle)
    {
        glDeleteBuffers(1, &g_ElementsHandle);
    }
    if (g_ShaderHandle)
    {
        glDeleteProgram(g_ShaderHandle);
    }
    g_VboHandle = 0;
    g_ElementsHandle = 0;
    g_ShaderHandle = 0;

    GLGui::DestroyFontsTexture();
}
