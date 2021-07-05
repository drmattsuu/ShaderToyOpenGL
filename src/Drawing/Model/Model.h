#pragma once

#include "Mesh.h"

#include <assimp/scene.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <memory>

class Model
{
public:
    Model(const std::string& filePath, bool gammaCorrection = false);

    void Draw(GLuint shader);

private:
    void LoadModel(const std::string& path);
    void UnloadModel();
    void ProcessNode(aiNode* node, const aiScene* scene);
    MeshPtr ProcessMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

private:
    Texture m_defaultTexture;
    std::vector<Texture> m_textures;
    std::vector<MeshPtr> m_meshes;
    std::string m_directory{};

    bool m_gammaCorrection{false};
};

using ModelPtr = std::shared_ptr<Model>;
