// OpenGLScratch.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "render/render_scene.h"
#include "rslib.h"

int main(int argc, char**argv)
{
    RSLib::instance()->initConfig(argc, argv);
    basic_lighting::basic_lighting_main();
}
