#pragma once
#include <memory>

class Render;

class Engine {
    std::unique_ptr<Render> m_pRender;
public:
    Engine() = delete;
    Engine(int argc, char** argv);
    ~Engine();

public:
    int initWindow();
    int closeWindow();
    int run();

private:


    bool m_bWindow;

};

