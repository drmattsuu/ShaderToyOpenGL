#pragma once

#include "GL/glew.h"

#include <string>

GLuint LoadShaderFile(const std::string& vertPath, const std::string& fragPath);
GLint CompileShaderSrc(GLuint shaderId, const std::string& shaderSrc);
GLint LinkShader(GLuint programId, GLuint vertShaderId, GLuint fragShaderId);

GLuint LoadTextureBMPFile(const std::string& bmpPath);
