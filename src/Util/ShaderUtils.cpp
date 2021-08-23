#include "ShaderUtils.h"

#include "FileUtils.h"

#include "lodepng.h"

#include <GL/glew.h>

#include <iostream>
#include <string>
#include <vector>

// Required to ensure STB is implemented
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace
{
constexpr char k_UseDefinesToken[] = "//{USE_DEFINES}";

void StringReplace(std::string& in, const std::string& find, const std::string& replace)
{
    size_t pos = 0;
    while ((pos = in.find(find, pos)) != std::string::npos)
    {
        in.replace(pos, find.length(), replace);
        pos += replace.length();
    }
}
}  // namespace

GLuint LoadShaderFile(const std::string& vertPath, const std::string& fragPath, const std::string& defines /*= ""*/)
{
    std::string vertSrc = LoadFileText(vertPath);
    std::string fragSrc = LoadFileText(fragPath);
    if (vertSrc.empty() || fragSrc.empty())
    {
        // Failed to get files see terminal for error message.
        return 0;
    }

    StringReplace(vertSrc, k_UseDefinesToken, defines);
    StringReplace(fragSrc, k_UseDefinesToken, defines);

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

GLuint LoadTextureBMPFile(const std::string& bmpPath)
{
    std::vector<char> file = LoadFileBin(bmpPath);

    if (file.size() < 54 || file[0] != 'B' && file[1] != 'M')
    {
        std::cout << "BMP file invalid: " + bmpPath << std::endl;
        return 0;
    }

    if ((*(int*)&(file[0x1E]) != 0) && (*(int*)&(file[0x1C]) != 24))
    {
        std::cout << "BMP file: " + bmpPath << " is not in the 24bpp format." << std::endl;
        return 0;
    }

    // Read the information about the image
    unsigned dataPos = *(int*)&(file[0x0A]);
    unsigned imageSize = *(int*)&(file[0x22]);
    unsigned width = *(int*)&(file[0x12]);
    unsigned height = *(int*)&(file[0x16]);

    // Some BMP files are misformatted, guess missing information
    if (imageSize == 0)
        imageSize = width * height * 3;  // 3 : one byte for each Red, Green and Blue component
    if (dataPos == 0)
        dataPos = 54;  // width of the BMP header

    GLuint prevTextureBinding2D;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&prevTextureBinding2D);

    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, &file[dataPos]);
    // Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // MipMaps
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, prevTextureBinding2D);
    return textureId;
}

GLuint LoadTexturePNGFile(const std::string& pngFile)
{
    // Due to modern hardware & versions of OGL we can use NPOT Textures, in earlier versions of OGL or old hardware we
    // may have had to force this texture to be in a power of two.
    std::vector<unsigned char> image;
    unsigned width, height;
    unsigned error = lodepng::decode(image, width, height, pngFile);

    if (error != 0)
    {
        std::cout << pngFile << " error: " << error << " : " << lodepng_error_text(error) << std::endl;
        return 0;
    }

    GLuint prevTextureBinding2D;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&prevTextureBinding2D);

    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
    // Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // MipMaps
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, prevTextureBinding2D);
    return textureId;
}

GLuint LoadTextureSTB(const std::string& filePath, bool gammaCorrection)
{
    GLuint textureId = 0;
    glGenTextures(1, &textureId);

    int width, height, nComponents;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nComponents, 0);
    if (data)
    {
        GLenum format;
        if (nComponents == 1)
            format = GL_RED;
        else if (nComponents == 3)
            format = GL_RGB;
        else if (nComponents == 4)
            format = GL_RGBA;
        else
            format = GL_BLUE;  // something blindingly obvious has gone wrong.

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << filePath << std::endl;
        stbi_image_free(data);
    }

    return textureId;
}

GLuint LoadCubeMapSTB(std::vector<std::string> faces)
{
    if (faces.size() != 6)
    {
        std::cout << "Attribute Error: LoadCubeMap expected 6 faces, got " << faces.size() << std::endl;
        return 0;
    }

    GLuint texId = 0;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

    int width, height, nComponents;
    for (unsigned int i = 0; i < faces.size(); ++i)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nComponents, 0);
        if (data)
        {
            GLenum format;
            if (nComponents == 1)
                format = GL_RED;
            else if (nComponents == 3)
                format = GL_RGB;
            else if (nComponents == 4)
                format = GL_RGBA;
            else
                format = GL_BLUE;  // something blindingly obvious has gone wrong.
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE,
                         data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
        }
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texId;
}
