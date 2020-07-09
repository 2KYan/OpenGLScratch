#include "render.h"

#include "rslib.h"
#include "camera.h"
#include "config.h"

#include "shader.h"
#include "texture.h"
#include "camera.h"

#include "model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <map>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>

#include "stb_image.h"

#include "spdlog/spdlog.h"


Render::Render()
{
    auto config = RSLib::instance()->getConfig();
    m_scr_width = config->width();
    m_scr_height = config->height();
    m_camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), float(m_scr_width)/m_scr_height);

    m_lastX = m_scr_width / 2.0f;
    m_lastY = m_scr_height / 2.0f;

    m_firstMouse = true;
    m_pressedMouseButton = 0;
    m_show_fhps = false;

    // timing
    m_deltaTime = 0.0f;	// time between current frame and last frame
    m_lastFrame = 0.0f;
}


Render::~Render()
{
}

void Render::fb_resize_callback(GLFWwindow* window, int width, int height)
    {
    glViewport(0, 0, width, height);
}

void Render::mouse_click_callback(GLFWwindow* window, int button, int action, int mode)
{
    printf("Button: %x %x %x\n", button, action, mode);
    if (action) {
        m_pressedMouseButton = 1 << button;
    } else {
        m_pressedMouseButton = 0;
    }
}

void Render::mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (m_firstMouse) {
        m_lastX = float(xpos);
        m_lastY = float(ypos);
        m_firstMouse = false;
    }

    float xoffset = float(xpos) - m_lastX;
    float yoffset = m_lastY - float(ypos); // reversed since y-coordinates go from bottom to top

    m_lastX = float(xpos);
    m_lastY = float(ypos);

    if (m_pressedMouseButton == 1) {
        m_camera->Rotate(int(xpos), int(ypos), int(xoffset), int(yoffset), m_scr_width, m_scr_height); 
    } else {
        m_camera->ProcessMouseMovement(xoffset, yoffset);
    }
}

void Render::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    m_camera->ProcessMouseScroll(float(yoffset));
}

void Render::processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_camera->ProcessKeyboard(CameraMovement::FORWARD, m_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_camera->ProcessKeyboard(CameraMovement::BACKWARD, m_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_camera->ProcessKeyboard(CameraMovement::LEFT, m_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_camera->ProcessKeyboard(CameraMovement::RIGHT, m_deltaTime);
}
int Render::run()
{
    init();
    prepare();
    render();
    cleanup();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

int Render::init()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    m_window = glfwCreateWindow(m_scr_width, m_scr_height, "OpenGL", NULL, NULL);
    if (m_window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(m_window);

    glfwSetWindowUserPointer(m_window, reinterpret_cast<void*>(this));

    auto resize_cb = [](GLFWwindow* w, int width, int height) {
        reinterpret_cast<Render*>(glfwGetWindowUserPointer(w))->fb_resize_callback(w, width, height);
    };

    glfwSetFramebufferSizeCallback(m_window, resize_cb);

    auto mouse_clk_cb = [](GLFWwindow* w, int button, int action, int mode) {
        reinterpret_cast<Render*>(glfwGetWindowUserPointer(w))->mouse_click_callback(w, button, action, mode);
    };
    glfwSetMouseButtonCallback(m_window, mouse_clk_cb);

    auto mouse_mv_cb = [](GLFWwindow* w, double xpos, double ypos) {
        reinterpret_cast<Render*>(glfwGetWindowUserPointer(w))->mouse_move_callback(w, xpos, ypos);
    };
    glfwSetCursorPosCallback(m_window, mouse_mv_cb);

    auto scroll_cb = [](GLFWwindow* w, double xoffset, double yoffset) {
        reinterpret_cast<Render*>(glfwGetWindowUserPointer(w))->scroll_callback(w, xoffset, yoffset);
    };
    glfwSetScrollCallback(m_window, scroll_cb);

    // tell GLFW to capture our mouse
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader(( GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);


    return 0;
}

int Render::clear_exit(std::string message)
    {
    std::cout << message << std::endl;
    glfwTerminate();
    return -1;
}

int Render::prepare()
{
    auto config = RSLib::instance()->getConfig();

    // load model
    auto models = config->get_object_names("model");
    for (auto& m : models) {
        std::string path = "model/" + m;
        m_model.emplace_back(std::make_shared<Model>(m, path));
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Render to FBO
    glGenFramebuffers(1, &m_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

    glGenTextures(1, &m_textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_scr_width, m_scr_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureColorBuffer, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_scr_width, m_scr_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        spdlog::error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    return 0;
}

int Render::render()
{
    std::unordered_map<std::string, std::vector<glm::vec3>> offset_table;

    offset_table["cube"] = 
        {   glm::vec3(-1.0f, 0.0f, -1.0f),
            glm::vec3(2.0f, 0.0f, 0.0f),
        };

    offset_table["panel"] = 
        {   glm::vec3(-1.5f, 0.0f, -0.48f),
            glm::vec3(1.5f, 0.0f, 0.51f),
            glm::vec3(0.0f, 0.0f, 0.7f),
            glm::vec3(-0.3f, 0.0f, -2.3f),
            glm::vec3(0.5f, 0.0f, -0.6f) 
        };
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(m_window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        m_deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;

        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
        lightPos.x = 2.0f * sin(currentFrame);
        lightPos.y = -0.3f; // *sin(currentFrame) + cos(currentFrame);
        lightPos.z = 1.5f * cos(currentFrame);
        // input
        // -----
        processInput(m_window);

        // render
        // ------
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glm::mat4 projection = glm::perspective(glm::radians(m_camera->Zoom), (float)m_scr_width/ (float)m_scr_height, 0.1f, 100.0f);
        glm::mat4 view = m_camera->GetViewMatrix();
        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // it's a bit too big for our scene, so scale it down

        for (auto& m : m_model) {
            if (m->check("framebuffer")) {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glDisable(GL_DEPTH_TEST);
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
                glClear(GL_COLOR_BUFFER_BIT);

                glBindTexture(GL_TEXTURE_2D, m_textureColorBuffer);
                m->draw();
            } else {
                if (offset_table.find(m->name()) != offset_table.end()) {
                    std::map<float, glm::vec3> ordered;
                    for (auto& t : offset_table[m->name()]) {
                        float distance = glm::length(m_camera->Position - t);
                        ordered[distance] = t;
                    }
                    for (auto it = ordered.rbegin(); it != ordered.rend();it++) {
                        model = glm::mat4(1.0f);
                        model = glm::translate(model, it->second);

                        m->draw(model, view, projection);
                    }

                } else {
                    m->draw(model, view, projection);
                }
            }
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
    return 0;
}

int Render::cleanup()
{

    return 0;
}
