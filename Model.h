#pragma once
class Model
{
public:
    Model(char* path);
    void Draw();
private:
    vector<Mesh> m_Meshes;
   
    void loadModel(string path);

};

