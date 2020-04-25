#include "rslib.h"

#if defined(_WIN32)
#include <windows.h>
#include <locale>
#include <codecvt>
#elif defined (LINUX)
#include <sys/wait.h>
#include <unistd.h>
#endif

#include <sys/stat.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <mutex>

//#include "shaderc/shaderc.hpp"
#pragma warning(disable:4996)

RSLib* RSLib::instance()
{ 
    std::mutex inst_m;
    std::lock_guard<std::mutex> l(inst_m);

    static std::unique_ptr<RSLib> thisPtr(new RSLib);
    static bool isInit = false;
    if (isInit == false) {
        isInit = true;
        thisPtr->initResPaths();
    }

    return thisPtr.get();
}

RSLib::RSLib() 
{
    resTypeStrings.assign({ "shader", "texture", "model" });
    m_enableSPVDump = false;
}

RSLib::~RSLib()
{
}

int RSLib::initResPaths()
{
    char* envPath = nullptr;
    std::vector<std::string> paths = { "data", "../data", "../shared" };

    std::unordered_map<std::string, std::string> envPaths = {
        { "DEV_HOME", "/data"},
    };

    for (const auto& envVar : envPaths) {
        if ((envPath = getenv(envVar.first.c_str())) != nullptr) {
            paths.push_back(std::string(envPath) + envVar.second);
        }
    }

    std::string curPath;
    struct stat buffer;
    for (auto& resType : resTypeStrings) {
        if (resPaths.find(resType) != resPaths.end()) {
            resPaths[resType].clear();
        }
        for (const auto& path : paths) {
            curPath = path + "/";
            if (stat(curPath.c_str(), &buffer) == 0) {
                resPaths[resType].push_back(curPath);
            }
            curPath = path + "/" + resType + "/";
            if (stat(curPath.c_str(), &buffer) == 0) {
                resPaths[resType].push_back(curPath);
            }
        }
    }

    return 0;
}

int RSLib::numResPaths()
{
    int numPaths = 0;
    for (auto& p: resPaths) {
        numPaths += int(p.second.size());
    }
    return numPaths;
}

std::string RSLib::getModelFileName(const char* fileName)
{
    if (fileName == nullptr)
        return std::string();
    else
        return getResourceFileName(fileName, "model");
}

std::string RSLib::getShaderFileName(const char* fileName) {
    if (fileName == nullptr)
        return std::string();
    else
        return getResourceFileName(fileName, "shader");
}

std::string RSLib::getTextureFileName(const char* fileName) {
    if (fileName == nullptr)
        return std::string();
    else
        return getResourceFileName(fileName, "texture");
}

std::string RSLib::getResourceFileName(const std::string& fileName, std::string resType)
{
    std::transform(resType.begin(), resType.end(), resType.begin(), ::tolower);
    if (fileName == "") return std::string();
    struct stat buffer;
    for (const auto& resPath : resPaths[resType]) {
        if (stat((resPath + fileName).c_str(), &buffer) == 0) {
            return resPath + fileName;
        }
    }

    return std::string();
}

std::string RSLib::getFileNameWoExt(const std::string& fileName)
{
    std::string fileNameStr(fileName);
    std::string sep("\\");
    size_t offset = 0;
    while ((offset = fileNameStr.find(sep, offset)) != std::string::npos) {
        fileNameStr.replace(offset, sep.length(), "/");
    }
    if ((offset = fileNameStr.rfind("/", fileNameStr.length())) != std::string::npos) {
        fileNameStr.erase(0, offset + 1);
    }

    return fileNameStr;
}

std::string RSLib::getSpvFileName(const std::string& fileName)
{
    std::string fileNameStr(getFileNameWoExt(fileName));
    size_t offset = 0;
    while ((offset = fileNameStr.find('.', offset)) != std::string::npos) {
        fileNameStr.replace(offset, 1, 1, '-');
    }
    return fileNameStr + ".spv";
}

std::stringstream RSLib::glslCompile(const char* fileName, int shader_type)
{
    std::stringstream stream;
    size_t size = 0;
    std::vector<uint32_t> spvBinary = glslCompile(fileName, size, shader_type);

    if (spvBinary.size() != 0) {
        stream.rdbuf()->sputn((char*)spvBinary.data(), size);
    }

    return stream;
}

std::vector<uint32_t> RSLib::glslCompile(const char* fileName, size_t& size, int shader_type)
{
    std::vector<uint32_t> spvBinary;

    //shaderc::Compiler compiler;
    //shaderc::CompileOptions options;

    std::string fName;

    if ((fName = getShaderFileName(fileName)).empty() == false) {
        std::ifstream t(fName);
        std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

        //options.AddMacroDefinition("MY_DEFINE", "1");
        /*
        shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(str, static_cast<shaderc_shader_kind>(shader_type), "shader_src", options);

        if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
            std::string msg = module.GetErrorMessage();
        } else {
            spvBinary.assign(module.cbegin(), module.cend()); 
            size = (module.cend() - module.cbegin()) * sizeof(uint32_t);

            static int count = 0;
            if (m_enableSPVDump) {
                std::string fileName("shader");
                fileName += std::to_string(count++) + ".spv";
                FILE* fptr = fopen(fileName.c_str(), "wb");
                fwrite(spvBinary.data(), size, 1, fptr);
                fclose(fptr);
            }
        }
        */
    }

    return spvBinary;

}

void* RSLib::glslRead(const char* fileName, size_t& size) 
{
    std::string glslFileName;
    std::string outSpvFileName;

    if ((glslFileName = getShaderFileName(fileName)).empty() == false) {
        outSpvFileName = getSpvFileName(glslFileName);
        if (glsl2spv(glslFileName, outSpvFileName) == 0) {
            FILE* fp = fopen(outSpvFileName.c_str(), "rb");
            if (!fp) {
                return nullptr;
            }

            fseek(fp, 0L, SEEK_END);
            size = ftell(fp);

            fseek(fp, 0L, SEEK_SET);

            void* shader_code = malloc(size);
            size_t retval = fread(shader_code, size, 1, fp);
            fclose(fp);
            
            return shader_code;
        }
    }
    return nullptr;
}

int RSLib::glsl2spv(const std::string& glslFileName, const std::string& outSpvFileName)
{
    std::string cmdLine = std::string("glslangValidator -s -V -o") + outSpvFileName + " " + glslFileName;
    return execCmd(cmdLine);
}

int RSLib::execCmd(std::string& cmd) 
{
    int rtnVal = 0;
#if defined(_WIN32)
    PROCESS_INFORMATION p_info;
    STARTUPINFO s_info;

    memset(&p_info, 0, sizeof(p_info));
    memset(&s_info, 0, sizeof(s_info));
    s_info.cb = sizeof(s_info);

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring widestring = converter.from_bytes(cmd);

    if (CreateProcess(NULL, const_cast<LPWSTR>(widestring.c_str()), NULL, NULL, 0, CREATE_NO_WINDOW, NULL, NULL, &s_info, &p_info)) {
        WaitForSingleObject(p_info.hProcess, INFINITE);
        CloseHandle(p_info.hProcess);
        CloseHandle(p_info.hThread);
    } else {
        rtnVal = -1;
    }
#elif defined(LINUX)
    pid_t pid = fork();
    int status;
    switch(pid) {
    case -1: //error
        rtnVal = -1;
        break;
    case 0: //chil process
        execl(cmd.c_str(), 0, 0); //run the command
        exit(1);
    default: //parent process, pid now contains the child pid
        while(-1 == waitpid(pid, &status, 0)); //wait for child to complete
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            std::cerr << "process " << cmd << "(pid=" << pid << ") failed" << std::endl;
        } else {
            rtnVal = 0;
        }
        break;
    }
#endif
    return rtnVal;
}

