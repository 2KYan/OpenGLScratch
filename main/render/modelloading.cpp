#include "ModelLoading.h"

#include "rslib.h"

#include "shader.h"
#include "texture.h"
#include "camera.h"

#include "model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int ModelLoading::prepare()
{
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // load model
    std::string model_name = RSLib::instance()->getModelFileName("rock/rock.obj");
    m_model = std::make_shared<Model>(model_name.c_str());
    printf("%s\n", model_name.c_str());

    // build and compile our shader zprogram
    // ------------------------------------
    m_Shader = std::make_shared<Shader>("modelloading.vert", "modelloading.frag");

    return 0;
}

int ModelLoading::render()
{

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
        m_Shader->use();

        glm::mat4 projection = glm::perspective(glm::radians(m_camera->Zoom), (float)m_scr_width/ (float)m_scr_height, 0.1f, 100.0f);
        glm::mat4 view = m_camera->GetViewMatrix();
        m_Shader->setMat4("projection", projection);
        m_Shader->setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // it's a bit too big for our scene, so scale it down
        m_Shader->setMat4("model", model);
        m_model->Draw(m_Shader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
    return 0;
}

int ModelLoading::cleanup()
{

    return 0;
}
