#pragma once
#include "renderBase.h"

class Shader;
class ModelLoading : public RenderBase {

public:
    int prepare();
    int render();
    int cleanup();

public:
    std::shared_ptr<Shader> m_ObjShader;
    std::shared_ptr<Shader> m_LightShader;
    unsigned VBO;
    unsigned cubeVAO;
    unsigned lightVAO;
};
