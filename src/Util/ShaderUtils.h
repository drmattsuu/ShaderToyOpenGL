#pragma once

#include "GL/glew.h"

#include <string>

GLuint LoadShaderFile(const std::string& vertPath, const std::string& fragPath, const std::string& defines = "");
GLint CompileShaderSrc(GLuint shaderId, const std::string& shaderSrc);
GLint LinkShader(GLuint programId, GLuint vertShaderId, GLuint fragShaderId);

GLuint LoadTextureBMPFile(const std::string& bmpPath);
GLuint LoadTexturePNGFile(const std::string& pngFile);

GLuint LoadTextureSTB(const std::string& filePath, bool gammaCorrection);
