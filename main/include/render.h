#pragma once
#include "renderBase.h"

class Shader;
class Model;
class Render : public RenderBase {

public:
    int prepare();
    int render();
    int cleanup();

public:
    std::shared_ptr<Model> m_model;
    std::shared_ptr<Shader> m_Shader;
};
