#pragma once
#include "renderBase.h"
class Simple : public RenderBase{

public:
    int prepare();
    int render();
    int cleanup();
};
