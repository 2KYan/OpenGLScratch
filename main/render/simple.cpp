#include "simple.h"

#include "shader.h"
#include "texture.h"
#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int Simple::prepare()
{
    return 0;
}

int Simple::render()
{

struct Vertex {
    float x, y, z;
    float r, g, b, a;
    float s, t;
};

    Shader shader("simple_triangle.vert", "simple_triangle.frag");
    shader.use();
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);

    Vertex vertices[] = {
     0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
     0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // bottom left
    -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    auto tex1 = std::make_unique<Texture>();
    tex1->loadTexture("container.jpg");

    auto tex2 = std::make_unique<Texture>();
    tex2->loadTexture("wall.jpg");

    //glEnable(GL_DEPTH_TEST);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    unsigned int VBO, VAO, VEO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VEO);
    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VEO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(void*)(offsetof(Vertex, x)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, r)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, s)));
    glEnableVertexAttribArray(2);


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

    while (!glfwWindowShouldClose(m_window)) {
        float currentFrame = static_cast<float>(glfwGetTime());

        processInput(m_window);


        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float colorShift = (sin(currentFrame) / 2.0f) + 0.5f;
        shader.use();
        shader.setVec4("colorShift", glm::vec4(glm::vec3(colorShift), 1.0));

        glBindVertexArray(VAO);
        
        glm::mat4 projection = glm::perspective(glm::radians(m_camera->Zoom), ( float) m_scr_width/ ( float) m_scr_height, 0.1f, 100.0f);
        glm::mat4 view = m_camera->GetViewMatrix();

        // pass transformation matrices to the shader

        for (unsigned int i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("transform", projection* view * model);

            //glDrawArrays(GL_TRIANGLES, 0, 6);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
    return 0;
}

int Simple::cleanup()
{
    return 0;
}
