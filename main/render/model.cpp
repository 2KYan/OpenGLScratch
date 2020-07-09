#include <iostream>
#include "rslib.h"
#include "config.h"
#include "Model.h"
#include "texture.h"
#include "shader.h"

Model::Model(std::string model_name, std::string path, bool gamma)
{
    m_objname = model_name;
    m_path = path;

    auto config = RSLib::instance()->getConfig();

    for (auto on : config->get_object_names(path)) {
        auto obj = config->get_obj(path + "/" + on);
        if (obj->IsBool()) {
            m_settings[on] = obj->GetBool();    
        }
    }

    if (enable()) {
        std::string model_resource = path + "/resource";
        std::string shader_vs = path + "/shader/vs";
        std::string shader_fs = path + "/shader/fs";

        std::string model_path = RSLib::instance()->getModelFileName(config->get_string(model_resource).c_str());
        printf("%s\n", model_path.c_str());
        loadModel(model_path);

        auto vs = config->get_string(shader_vs);
        auto fs = config->get_string(shader_fs);
        m_shader = std::make_shared<Shader>(vs.c_str(),fs.c_str());
        
        if (check("framebuffer")) {
            m_shader->use();
            m_shader->setInt("screenTexture", 0);
        }
    }
    

    gammaCorrection = gamma;
}

void Model::draw()
{
    if (enable()) {
        m_shader->use();

        for (auto& mesh : m_meshes) {
            mesh.Draw(m_shader);
        }
    }

}

void Model::draw(glm::mat4 model, glm::mat4 view, glm::mat4 proj)
{
    if (enable()) {
        m_shader->use();

        m_shader->setMat4("projection", proj);
        m_shader->setMat4("view", view);
        m_shader->setMat4("model", model);

        for (auto& mesh : m_meshes) {
            mesh.Draw(m_shader);
        }
    }
}

bool Model::enable() {
    return m_settings["disable"] == false;
}

bool Model::check(std::string attrib)
{
    return m_settings[attrib];
}

void Model::loadModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs |aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "Error::Assimp::" << importer.GetErrorString() << std::endl;
    }

    directory = path.substr(0, path.find_last_of("/"));
    processNode(scene->mRootNode, scene);

}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) 
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i) 
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture_t> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (mesh->HasNormals()) {
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        if (mesh->HasTextureCoords(0)) {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        if (mesh->HasTangentsAndBitangents()) {
            vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0) {
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps
        std::vector<Texture_t> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<Texture_t> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture_t> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture_t> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture_t> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture_t> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (unsigned int j = 0; j < m_textures.size(); j++) {
            if (std::strcmp(m_textures[j].name.data(), str.C_Str()) == 0) {
                textures.push_back(m_textures[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip) {   // if texture hasn't been loaded already, load it
            Texture_t texture;
            Texture tex;
            texture.id = tex.loadTexture(str.C_Str());
            texture.type = typeName;
            texture.name = str.C_Str();
            textures.push_back(texture);
            m_textures.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    return textures;
}
