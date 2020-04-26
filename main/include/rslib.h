#ifndef _RSLIB_H
#define _RSLIB_H

#include <string>
#include <vector>
#include <unordered_map>

class RSLib {
public:
    struct args {
        int m;
        int n;
        int k;
        int verbose;
    };

public:
    virtual ~RSLib();
    static RSLib* instance();

    struct args parse_arguments(int argc, char** argv);
    std::string getModelFileName(const char* fileName);
    std::string getShaderFileName(const char* fileName);
    std::string getTextureFileName(const char* fileName);
    int execCmd(std::string & cmd);
    void* glslRead(const char* fileName, size_t& size);
    std::vector<uint32_t> glslCompile(const char* fileName, size_t& size, int shader_type);
    std::stringstream glslCompile(const char* fileName, int shader_type);

    int numResPaths();
protected:
    int initResPaths();
    std::string getResourceFileName(const std::string& fileName, std::string resType);
    std::string getFileNameWoExt(const std::string& fileName);
    std::string getSpvFileName(const std::string& fileName);
    int glsl2spv(const std::string& glslFileName, const std::string& spvFileName);

private:
    RSLib();
    RSLib(RSLib const&) = delete;
    RSLib& operator=(RSLib const &) = delete;
    RSLib(RSLib &&) = delete;
    RSLib& operator=(RSLib &&) = delete;
    
private:
    bool m_enableSPVDump;
    std::vector<std::string> resTypeStrings;
    std::unordered_map<std::string, std::vector<std::string>> resPaths;
};


#endif //_RSLIB_H