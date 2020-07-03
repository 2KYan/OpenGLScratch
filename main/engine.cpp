#include "engine.h"
#include "render.h"
#include "rslib.h"

Engine::Engine(int argc, char** argv)
{
    RSLib::instance()->initConfig(argc, argv);
    m_bWindow = false;


    m_pRender = std::unique_ptr<Render>(new Render());
}

Engine::~Engine()
{
    if (m_bWindow) {
        closeWindow();
    }
}

int Engine::initWindow()
{
    return 0;
}

int Engine::closeWindow()
{
    return 0;
}

int Engine::run()
{
    return m_pRender->run();
}
