#pragma once
#include "render.h"
class Simple : public Render{

public:
    int prepare();
    int render();
    int cleanup();
};
