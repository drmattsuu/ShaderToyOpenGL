#include "Model.h"

#include "Util/ShaderUtils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <iostream>

Model::Model(const std::string& filePath, bool gammaCorrection /*= false*/)
{
    m_defaultTexture.id = LoadTextureBMPFile("resources/default.bmp");
    m_defaultTexture.path = "resources/default.bmp";
    m_defaultTexture.type = k_TextureTypeDiffuse;
    m_textures.push_back(m_defaultTexture);

    LoadModel(filePath);
}

Model::~Model()
{
    UnloadModel();
}

void Model::Draw(GLuint shader)
{
    for (MeshPtr& m : m_meshes)
        m->Draw(shader);
}

void Model::LoadModel(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    // retrieve the directory path of the filepath
    m_directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    ProcessNode(scene->mRootNode, scene);
}

void Model::UnloadModel()
{
    m_meshes.clear();
    for (Texture tex : m_textures)
    {
        glDeleteTextures(1, &tex.id);
    }
    m_textures.clear();
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(ProcessMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

MeshPtr Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // we declare a placeholder vector since assimp uses its own vector class that doesn't directly
        // convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        glm::vec3 vector;
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0])  // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex
    // indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, k_TextureTypeDiffuse);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, k_TextureTypeSpecular);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, k_TextureTypeNormal);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, k_TextureTypeHeight);
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    if (textures.empty())
    {
        textures.push_back(m_defaultTexture);
    }

    // return a mesh object created from the extracted mesh data
    return MeshPtr(new Mesh(vertices, indices, textures));
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (unsigned int j = 0; j < m_textures.size(); j++)
        {
            if (std::strcmp(m_textures[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(m_textures[j]);
                skip = true;  // a texture with the same filepath has already been loaded, continue to next one.
                              // (optimization)
                break;
            }
        }
        if (!skip)
        {  // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = LoadTextureSTB(m_directory + '/' + str.C_Str(), m_gammaCorrection);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            m_textures.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't
                                            // unnecessary load duplicate textures.
        }
    }
    return textures;
}
