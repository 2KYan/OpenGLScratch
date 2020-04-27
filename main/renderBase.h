#pragma once

#include<istream>
#include<fstream>
#include <string>

#include "glad/glad.h"
#define GLFW_DLL
#include "glfw/glfw3.h"

class Camera;
class Shader;

class RenderBase {
struct Vertex {
    float x, y, z;
    float r, g, b;
    float s, t;
};

public:
    RenderBase();
    virtual ~RenderBase();

public:
    void fb_resize_callback(GLFWwindow* window, int width, int height);
    void mouse_click_callback(GLFWwindow* window, int button, int action, int mode);
    void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    void processInput(GLFWwindow* window);
    
    int init();
    int prepare();
    int run();
    int cleanup();
    int clear_exit(std::string message);

protected:
    std::string loadShader(const std::string filename)
    {
        std::ifstream ifs(filename);
        return std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    }

protected:
    std::shared_ptr<Shader> m_ObjShader;
    std::shared_ptr<Shader> m_LightShader;
    unsigned VBO;
    unsigned cubeVAO;
    unsigned lightVAO;

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

    GLFWwindow* m_window;

};
