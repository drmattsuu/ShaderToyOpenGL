#include "ShaderUtils.h"

#include "FileUtils.h"

#include <GL/glew.h>

#include <iostream>
#include <string>
#include <vector>

GLuint LoadShaderFile(const std::string& vertPath, const std::string& fragPath)
{
    std::string vertSrc = LoadFileText(vertPath);
    std::string fragSrc = LoadFileText(fragPath);
    if (vertSrc.empty() || fragSrc.empty())
    {
        // Failed to get files see terminal for error message.
        return 0;
    }

    GLuint vertShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    CompileShaderSrc(vertShaderId, vertSrc);
    CompileShaderSrc(fragShaderId, fragSrc);

    GLuint programId = glCreateProgram();
    LinkShader(programId, vertShaderId, fragShaderId);

    glDeleteShader(vertShaderId);
    glDeleteShader(fragShaderId);

    return programId;
}

GLint CompileShaderSrc(GLuint shaderId, const std::string& shaderSrc)
{
    GLint result = GL_FALSE;
    int logLength;

    GLchar const* shaderSrcPtr = shaderSrc.c_str();
    glShaderSource(shaderId, 1, &shaderSrcPtr, nullptr);
    glCompileShader(shaderId);

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        using SizeType = std::vector<char>::size_type;
        std::vector<char> logMsg(static_cast<SizeType>(logLength) + 1);
        glGetShaderInfoLog(shaderId, logLength, nullptr, &logMsg[0]);
        std::string msg(logMsg.begin(), logMsg.end());
        std::cout << msg << std::endl;
    }

    return result;
}

GLint LinkShader(GLuint programId, GLuint vertShaderId, GLuint fragShaderId)
{
    glAttachShader(programId, vertShaderId);
    glAttachShader(programId, fragShaderId);
    glLinkProgram(programId);

    GLint result = GL_FALSE;
    int logLength;
    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);

    if (logLength > 0)
    {
        using SizeType = std::vector<char>::size_type;
        std::vector<char> logMsg(static_cast<SizeType>(logLength) + 1);
        glGetProgramInfoLog(programId, logLength, nullptr, &logMsg[0]);
        std::string msg(logMsg.begin(), logMsg.end());
        std::cout << msg << std::endl;
    }

    glDetachShader(programId, vertShaderId);
    glDetachShader(programId, fragShaderId);

    return result;
}
