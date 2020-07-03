#pragma once
#include "render.h"

class Shader;

class BasicLighting : public Render{
public:
    BasicLighting();
    virtual ~BasicLighting(); 

public:
    virtual int prepare() override;
    virtual int render() override;
    virtual int cleanup() override;
    
protected:
    std::shared_ptr<Shader> m_ObjShader;
    std::shared_ptr<Shader> m_LightShader;
    unsigned VBO;
    unsigned cubeVAO;
    unsigned lightVAO;
};
