#include <iostream>
#include "rslib.h"
#include "config.h"
#include "texture.h"
#include "shader.h"
#include "model.h"

#include "renderpass.h"



RenderPass::RenderPass(std::string model_name, std::string path)
{
    m_objname = model_name;
    m_path = path;

    auto config = RSLib::instance()->getConfig();

    m_settings = config->get_object_settings(path);

    if (enable()) {
        std::string model_resource = path + "/resource";
        std::string shader_vs = path + "/shader/vs";
        std::string shader_fs = path + "/shader/fs";

        std::string model_path = RSLib::instance()->getModelFileName(config->get_string(model_resource).c_str());
        //loadModel(model_path);

        auto vs = config->get_string(shader_vs);
        auto fs = config->get_string(shader_fs);
        m_shader = std::make_shared<Shader>(vs.c_str(),fs.c_str());
        
        if (check("framebuffer")) {
            m_shader->use();
            m_shader->setInt("screenTexture", 0);
        }
    }
    
}

void RenderPass::draw(glm::mat4 model, glm::mat4 view, glm::mat4 proj)
{
    if (enable()) {
        m_shader->use();

        m_shader->setMat4("projection", proj);
        m_shader->setMat4("view", view);
        m_shader->setMat4("model", model);

        for (auto& m : m_models) {
            
        }
    }
}

bool RenderPass::enable() {
    return m_settings["disable"] == false;
}

bool RenderPass::check(std::string attrib)
{
    return m_settings[attrib];
}

