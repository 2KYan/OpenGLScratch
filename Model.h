#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

class Model
{
public:
    Model(char* path, bool gamma = false);
    void Draw(std::shared_ptr<Shader> shader);
private:
    std::vector<Texture_t> m_textures;
    std::vector<Mesh> m_meshes;
    std::string directory;
    bool gammaCorrection;
    /*  Functions   */
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture_t> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

