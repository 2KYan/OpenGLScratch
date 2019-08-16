#ifndef _TEXTURE_H
#define _TEXTURE_H

class Texture {
private: 
    int activeTextureId = 0;
    void reset() { activeTextureId = 0; }

public:
    unsigned int ID;
    ~Texture();
    Texture();

    int loadTexture(const char* filename);
protected:

private:

};

#endif //_TEXTURE_H