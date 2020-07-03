#pragma once
#include <memory>

class Render;

class Engine {
public:
    Engine(int argc, char** argv);
    virtual ~Engine();

public:
    int initWindow();
    int closeWindow();
    int run();

private:
    Engine() { Engine(0, nullptr); }


    bool m_bWindow;

    std::shared_ptr<Render> m_pRender;
};

