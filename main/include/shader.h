#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class Shader
{
protected:
    struct ShaderS {
        bool valid;
        std::string shader_fn;
        std::string shader_code;
        unsigned shader_id;
    };

public:
    unsigned int ID;
    Shader(const char* vs, const char* fs, const char* tcs = nullptr, const char* tes = nullptr, const char* gs = nullptr);
    // activate the shader
    // ------------------------------------------------------------------------
    void use();
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec2(const std::string &name, float x, float y) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w);
    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    void loadShader();
    void checkCompileErrors(unsigned int shader, std::string type);
protected:
    std::unordered_map<unsigned int, ShaderS> m_shaderTable;
};
#endif