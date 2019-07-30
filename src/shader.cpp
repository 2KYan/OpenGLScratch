
#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include <filesystem>

#include "shader.h"
#include "rslib.h"

const std::unordered_map<unsigned int, std::string> shaderNameString = {
};

Shader::Shader(const char* vs, const char* fs, const char* tcs, const char* tes, const char* gs)
{
    m_shaderTable.clear();

    ShaderS ss;
    ss = { (vs != nullptr),  RSLib::instance()->getShaderFileName(vs)};
    m_shaderTable[GL_VERTEX_SHADER] = ss;
    ss = { (fs != nullptr),  RSLib::instance()->getShaderFileName(fs)};
    m_shaderTable[GL_FRAGMENT_SHADER] = ss;
    ss = { (tcs != nullptr), RSLib::instance()->getShaderFileName(tcs)};
    m_shaderTable[GL_TESS_CONTROL_SHADER] = ss;
    ss = { (tes != nullptr), RSLib::instance()->getShaderFileName(tes)};
    m_shaderTable[GL_TESS_EVALUATION_SHADER] = ss;
    ss = { (gs != nullptr),  RSLib::instance()->getShaderFileName(gs)};
    m_shaderTable[GL_GEOMETRY_SHADER] = ss;

    loadShader();
}

void Shader::loadShader()
{
    const char* shaderCodePtr;
    std::string shaderCode;
    std::ifstream shaderFile;

    ID = glCreateProgram();

    for (auto& info : m_shaderTable)
    {
        if (info.second.valid == false) {
            continue;
        }
        //std::cout << std::experimental::filesystem::current_path() << std::endl;

        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            
            // open files
            shaderFile.clear();
            shaderFile.open(info.second.shader_fn.c_str());
            std::stringstream shaderStream;
            shaderStream << shaderFile.rdbuf();
            shaderFile.close();
            shaderCode = shaderStream.str();
            info.second.shader_code = shaderCode;
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }

        shaderCodePtr = shaderCode.c_str();
        unsigned int shaderID = glCreateShader(info.first);
        info.second.shader_id = shaderID;
        glShaderSource(shaderID, 1, &shaderCodePtr, NULL);
        glCompileShader(shaderID);
        checkCompileErrors(shaderID, info.second.shader_fn);
        glAttachShader(ID, shaderID);
    }
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    for (auto& info : m_shaderTable)
    {
        if (info.second.valid) {
            glDeleteShader(info.second.shader_id);
        }
    }
}

void Shader::use()
{
    glUseProgram(ID);
}
// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::checkCompileErrors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}