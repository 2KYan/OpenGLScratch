#pragma once

#include <string>
#include <vector>
#include <glm/glm/glm.hpp>

struct Vertex{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 TexCoords;
};

struct Texture
{
    unsigned int id;
    std::string type;
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    void Draw();
private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    void setupMesh();
};

