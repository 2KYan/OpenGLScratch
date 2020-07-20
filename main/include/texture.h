#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <vector>
#include <memory>

class Texture {
private: 
    int activeTextureId = 0;
    void reset() { activeTextureId = 0; }

public:
    unsigned int ID;
    ~Texture();
    Texture();

    int loadTexture(const char* filename);
    int loadCubemap(std::vector<std::string> faces);
protected:

private:

};

#endif //_TEXTURE_H