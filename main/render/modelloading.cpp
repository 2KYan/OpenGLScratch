#include "ModelLoading.h"

#include "shader.h"
#include "texture.h"
#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int ModelLoading::prepare()
{
    struct Vertex {
        float x, y, z;
        float r, g, b;
        float s, t;
    };

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    auto tex = std::make_unique<Texture>();
    tex->loadTexture("container2.png");
    tex->loadTexture("container2_specular.png");


    // build and compile our shader zprogram
    // ------------------------------------
    m_ObjShader = std::make_shared<Shader>("simple_transform.vert", "simple_light.frag");
    m_LightShader = std::make_shared<Shader>("simple_transform.vert", "constant_color.frag");

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

    // first, configure the cube's VAO (and VBO)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
    return 0;
}

int ModelLoading::render()
{
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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // be sure to activate shader when setting uniforms/drawing objects
        m_ObjShader->use();

        glm::vec3 lightColor;
        lightColor.x = sin(currentFrame * 2.0f);
        lightColor.y = sin(currentFrame * 0.7f);
        lightColor.z = sin(currentFrame * 1.3f);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence

        m_ObjShader->setVec3("viewPos", m_camera->Position);

        m_ObjShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        m_ObjShader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        m_ObjShader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        m_ObjShader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        m_ObjShader->setVec3("pointLights[0].position", pointLightPositions[0]);
        m_ObjShader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        m_ObjShader->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        m_ObjShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        m_ObjShader->setFloat("pointLights[0].constant", 1.0f);
        m_ObjShader->setFloat("pointLights[0].linear", 0.09f);
        m_ObjShader->setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        m_ObjShader->setVec3("pointLights[1].position", pointLightPositions[1]);
        m_ObjShader->setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        m_ObjShader->setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        m_ObjShader->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        m_ObjShader->setFloat("pointLights[1].constant", 1.0f);
        m_ObjShader->setFloat("pointLights[1].linear", 0.09f);
        m_ObjShader->setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        m_ObjShader->setVec3("pointLights[2].position", pointLightPositions[2]);
        m_ObjShader->setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        m_ObjShader->setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        m_ObjShader->setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        m_ObjShader->setFloat("pointLights[2].constant", 1.0f);
        m_ObjShader->setFloat("pointLights[2].linear", 0.09f);
        m_ObjShader->setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        m_ObjShader->setVec3("pointLights[3].position", pointLightPositions[3]);
        m_ObjShader->setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        m_ObjShader->setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        m_ObjShader->setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        m_ObjShader->setFloat("pointLights[3].constant", 1.0f);
        m_ObjShader->setFloat("pointLights[3].linear", 0.09f);
        m_ObjShader->setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        m_ObjShader->setVec3("spotLight.position", m_camera->Position);
        m_ObjShader->setVec3("spotLight.direction", m_camera->Front);
        m_ObjShader->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        m_ObjShader->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        m_ObjShader->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        m_ObjShader->setFloat("spotLight.constant", 1.0f);
        m_ObjShader->setFloat("spotLight.linear", 0.09f);
        m_ObjShader->setFloat("spotLight.quadratic", 0.032f);
        m_ObjShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        m_ObjShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        // material properties
        m_ObjShader->setInt("material.diffuse", 0);
        m_ObjShader->setInt("material.specular", 1); // specular lighting doesn't have full effect on this object's material
        m_ObjShader->setFloat("material.shininess", 32.0f);

        // view/projection transformations
        glm::mat4 projection = m_camera->GetProjectionMatrix();
        glm::mat4 view = m_camera->GetViewMatrix();
        glm::mat4 model = m_camera->GetModelMatrix();

        m_ObjShader->setMat4("model", model);
        m_ObjShader->setMat4("view", view);
        m_ObjShader->setMat4("projection", projection);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        for (unsigned int i = 0; i < 10; i++) {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 cubeModel = glm::mat4(1.0f);
            cubeModel = glm::translate(cubeModel, cubePositions[i]);
            float angle = 20.0f * i;
            cubeModel = glm::rotate(cubeModel, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            m_ObjShader->setMat4("model", cubeModel * model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        m_LightShader->use();
        m_LightShader->setMat4("view", view);
        m_LightShader->setMat4("projection", projection);

        glBindVertexArray(lightVAO);
        for (unsigned int i = 0; i < 4; i++) {
            lightPos.x = pointLightPositions[i].x * sin(currentFrame);
            lightPos.y = pointLightPositions[i].y;
            lightPos.z = pointLightPositions[i].z * cos(currentFrame);
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPos);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            m_LightShader->setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
    return 0;
}

int ModelLoading::cleanup()
{
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    return 0;
}
