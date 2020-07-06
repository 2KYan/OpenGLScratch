#pragma once
#include<istream>
#include<fstream>
#include <string>
#include <vector>

#include "glad/glad.h"
#define GLFW_DLL
#include "glfw/glfw3.h"

class Shader;
class Model;
class Camera;
class Render {
public:
    Render();
    virtual ~Render();

public:
    void fb_resize_callback(GLFWwindow* window, int width, int height);
    void mouse_click_callback(GLFWwindow* window, int button, int action, int mode);
    void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    void processInput(GLFWwindow* window);

    virtual int run() final;
    
    virtual int init() final;
    virtual int prepare();
    virtual int render();
    virtual int cleanup();
    int clear_exit(std::string message);

protected:
    int m_fps;
    unsigned m_scr_width = 1280;
    unsigned m_scr_height = 800;

    float m_lastX = m_scr_width / 2.0f;
    float m_lastY = m_scr_height / 2.0f;
    bool m_firstMouse = true;
    int  m_pressedMouseButton = 0;
    bool m_show_fhps = false;

    // timing
    float m_deltaTime = 0.0f;	// time between current frame and last frame
    float m_lastFrame = 0.0f;

    GLFWwindow* m_window;

    std::shared_ptr<Camera> m_camera;
    std::vector<std::shared_ptr<Model>> m_model;
};
