#pragma once 

#include "pch.h"
#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>

#include "glad/glad.h"
#include <glfw/glfw3.h>

#include "shader.h"
#include "texture.h"
#include "camera.h"

#include "render_scene.h"

#include "rslib.h"

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>


namespace basic_lighting
{

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 800;

Camera camera(glm::vec3(0.5f, 0.5f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), float(SCR_WIDTH)/SCR_HEIGHT);

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
int  pressedMouseButton = 0;
bool fps = false;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

struct Vertex {
    float x, y, z;
    float r, g, b;
    float s, t;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_click(GLFWwindow* window, int button, int action, int mode)
{
    printf("Button: %x %x %x\n", button, action, mode);
    if (action) {
        pressedMouseButton = 1 << button;
    } else {
        pressedMouseButton = 0;
    }
}

void mouse_move(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX = float(xpos);
        lastY = float(ypos);
        firstMouse = false;
    }

    float xoffset = float(xpos) - lastX;
    float yoffset = lastY - float(ypos); // reversed since y-coordinates go from bottom to top

    lastX = float(xpos);
    lastY = float(ypos);

    if (pressedMouseButton == 1) {
        camera.Rotate(int(xpos), int(ypos), int(xoffset), int(yoffset), SCR_WIDTH, SCR_HEIGHT); 
    } else {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(float(yoffset));
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

int clear_exit(std::string message)
{
    std::cout << message << std::endl;
    glfwTerminate();
    return -1;
}

std::string loadShader(const std::string filename) 
{
    std::ifstream ifs(filename);
    return std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
}

int basic_lighting_main()
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_move);
    glfwSetMouseButtonCallback(window, mouse_click);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader(( GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    auto tex = std::make_unique<Texture>();
    tex->loadTexture("container2.png");
    tex->loadTexture("container2_specular.png");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader objectShader("simple_transform.vert", "simple_light.frag");
    Shader lightSource("simple_transform.vert", "constant_color.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

    // first, configure the cube's VAO (and VBO)
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(void*)(offsetof(Vertex, x)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, r)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, s)));
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(void*)(offsetof(Vertex, x)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, r)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, s)));
    glEnableVertexAttribArray(2);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        lightPos.x = 2.0f  *sin(currentFrame);
        lightPos.y = -0.3f;// *sin(currentFrame) + cos(currentFrame);
        lightPos.z = 1.5f *cos(currentFrame);
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // be sure to activate shader when setting uniforms/drawing objects
        objectShader.use();

        glm::vec3 lightColor;
        lightColor.x = sin(currentFrame * 2.0f);
        lightColor.y = sin(currentFrame * 0.7f);
        lightColor.z = sin(currentFrame * 1.3f);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence

        objectShader.setVec3("viewPos", camera.Position);

        objectShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        objectShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        objectShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        objectShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        objectShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        objectShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        objectShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        objectShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        objectShader.setFloat("pointLights[0].constant", 1.0f);
        objectShader.setFloat("pointLights[0].linear", 0.09);
        objectShader.setFloat("pointLights[0].quadratic", 0.032);
        // point light 2
        objectShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        objectShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        objectShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        objectShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        objectShader.setFloat("pointLights[1].constant", 1.0f);
        objectShader.setFloat("pointLights[1].linear", 0.09);
        objectShader.setFloat("pointLights[1].quadratic", 0.032);
        // point light 3
        objectShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        objectShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        objectShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        objectShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        objectShader.setFloat("pointLights[2].constant", 1.0f);
        objectShader.setFloat("pointLights[2].linear", 0.09);
        objectShader.setFloat("pointLights[2].quadratic", 0.032);
        // point light 4
        objectShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        objectShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        objectShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        objectShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        objectShader.setFloat("pointLights[3].constant", 1.0f);
        objectShader.setFloat("pointLights[3].linear", 0.09);
        objectShader.setFloat("pointLights[3].quadratic", 0.032);
        // spotLight
        objectShader.setVec3("spotLight.position", camera.Position);
        objectShader.setVec3("spotLight.direction", camera.Front);
        objectShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        objectShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        objectShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        objectShader.setFloat("spotLight.constant", 1.0f);
        objectShader.setFloat("spotLight.linear", 0.09);
        objectShader.setFloat("spotLight.quadratic", 0.032);
        objectShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        objectShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        // material properties
        objectShader.setInt("material.diffuse", 0);
        objectShader.setInt("material.specular", 1); // specular lighting doesn't have full effect on this object's material
        objectShader.setFloat("material.shininess", 32.0f);

        // view/projection transformations
        glm::mat4 projection = camera.GetProjectionMatrix();
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = camera.GetModelMatrix();

        objectShader.setMat4("model", model);
        objectShader.setMat4("view", view);
        objectShader.setMat4("projection", projection);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        for (unsigned int i = 0; i < 10; i++) {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 cubeModel = glm::mat4(1.0f);
            cubeModel = glm::translate(cubeModel, cubePositions[i]);
            float angle = 20.0f * i;
            cubeModel = glm::rotate(cubeModel, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            objectShader.setMat4("model", cubeModel * model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        lightSource.use();
        lightSource.setMat4("view", view);
        lightSource.setMat4("projection", projection);

        glBindVertexArray(lightVAO);
        for (unsigned int i = 0; i < 4; i++) {
            lightPos.x = pointLightPositions[i].x * sin(currentFrame);
            lightPos.y = pointLightPositions[i].y;
            lightPos.z = pointLightPositions[i].z * cos(currentFrame);
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPos);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            lightSource.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


};
