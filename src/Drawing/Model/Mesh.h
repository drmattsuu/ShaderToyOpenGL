#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

constexpr char k_TextureTypeDiffuse[] = "texture_diffuse";
constexpr char k_TextureTypeSpecular[] = "texture_specular";
constexpr char k_TextureTypeNormal[] = "texture_normal";
constexpr char k_TextureTypeHeight[] = "texture_height";

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices,
         const std::vector<Texture>& textures);
    Mesh(const Mesh& rhs) = delete;
    ~Mesh();

    void Draw(GLuint shader);

private:
    void SetupMesh();
    void DestroyMesh();

private:
    GLuint m_vao{0};
    GLuint m_vbo{0};
    GLuint m_ebo{0};

    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture> m_textures;
};

using MeshPtr = std::shared_ptr<Mesh>;
