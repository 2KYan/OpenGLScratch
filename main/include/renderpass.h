#pragma once

#include <unordered_map>
#include "mesh.h"
class Shader;
class Model;

class RenderPass 
{
public:
    RenderPass(std::string model_name, std::string path);
    void draw(glm::mat4 model  = glm::mat4(1.0), glm::mat4 view = glm::mat4(1.0), glm::mat4 proj = glm::mat4(1.0)) ;

    std::string name()
    {
        return m_objname;
    };

    void use()
    {
        m_shader->use();
        m_shader->setInt("screenTexture", 0);
    }
    bool enable();
    bool check(std::string attrib);
private:
    std::string m_objname;
    std::string m_path;
    std::unordered_map<std::string, bool> m_settings;

    std::shared_ptr<Shader> m_shader;
    std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
    std::string directory;

    /*  Functions   */
    void load(std::string model_name);
    void loadShader(std::string path);

};

