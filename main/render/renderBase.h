#pragma once

#include<istream>
#include<fstream>
#include <string>
#define GLFW_DLL
#include "glfw/glfw3.h"

class Camera;

class RenderBase {
public:
    RenderBase(int width, int height);
    virtual ~RenderBase();

public:
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void mouse_click_callback(GLFWwindow* window, int button, int action, int mode);
    void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    void processInput(GLFWwindow* window);
    
    bool init();
    bool prepare();
    bool run();
    bool clear_exit(std::string message);

protected:
    std::string loadShader(const std::string filename)
    {
        std::ifstream ifs(filename);
        return std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    }



protected:
    int m_fps;
    unsigned m_scr_width = 1280;
    unsigned m_scr_height = 800;

    std::shared_ptr<Camera> m_camera;

    float m_lastX = m_scr_width / 2.0f;
    float m_lastY = m_scr_height / 2.0f;
    bool m_firstMouse = true;
    int  m_pressedMouseButton = 0;
    bool m_show_fhps = false;

    // timing
    float m_deltaTime = 0.0f;	// time between current frame and last frame
    float m_lastFrame = 0.0f;
};
