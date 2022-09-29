#pragma once

#include "GL/glew.h"

#include <string>
#include <vector>

GLuint LoadShaderFile(const std::string& vertPath, const std::string& fragPath, const std::string& defines = "");
GLuint LoadGeomShaderFile(const std::string& vertPath, const std::string& fragPath, const std::string& geomPath, const std::string& defines = "");
GLint CompileShaderSrc(GLuint shaderId, const std::string& shaderSrc);
GLint LinkShader(GLuint programId, GLuint vertShaderId, GLuint fragShaderId, GLuint geomShaderId = 0);

GLuint LoadTextureBMPFile(const std::string& bmpPath);
GLuint LoadTexturePNGFile(const std::string& pngFile);

GLuint LoadTextureSTB(const std::string& filePath, bool gammaCorrection);
GLuint LoadCubeMapSTB(std::vector<std::string> faces);
