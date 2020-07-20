#include "engine.h"
#include "render.h"
#include "rslib.h"

Engine::Engine(int argc, char** argv)
{
    m_bWindow = false;

    auto config = RSLib::instance()->initConfig(argc, argv);

    if (config) {
        m_pRender = std::unique_ptr<Render>(new Render());
    }

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
    if (m_pRender) {
        return m_pRender->run();
    } else {
        return -1;
    }
}
