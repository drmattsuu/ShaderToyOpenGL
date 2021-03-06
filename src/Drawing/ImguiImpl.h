#pragma once

#include "imgui.h"

struct GLFWwindow;

namespace GLGui
{
bool Init(GLFWwindow* window, bool installCallbacks);
void Shutdown();

void NewFrame();
void RenderDrawData(ImDrawData* drawData);

bool CreateFontsTexture();
void DestroyFontsTexture();
bool CreateDeviceObjects();
void DestroyDeviceObjects();
}  // namespace GLGui
