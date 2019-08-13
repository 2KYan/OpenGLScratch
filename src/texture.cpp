#include <iostream>

#include "glad/glad.h"
#include "texture.h"
#include "rslib.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int Texture::activeTextureId = 0;

Texture::Texture()
{

}

Texture::~Texture()
{

}

int Texture::loadTexture(const char* filename)
{
    glActiveTexture(GL_TEXTURE0 + activeTextureId);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    auto texture_fn = RSLib::instance()->getTextureFileName(filename);
    unsigned char* data = stbi_load(texture_fn.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    
    activeTextureId++;

    return activeTextureId;
}
