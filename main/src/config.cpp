#include "config.h"
#include <fstream>
#include <sstream>
#include <string>
#include "rapidjson/document.h"
#include "spdlog/spdlog.h"


template <class Container>
Container split(const std::string& str, char delim =' ')
{
    Container cont;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
    return cont;
}

Config::Config(std::string name)
{
    m_cfg_name = name;
    std::ifstream ifs(m_cfg_name);
    std::ostringstream contents;
    contents << ifs.rdbuf();

    if (m_doc.Parse(contents.str().c_str()).HasParseError()) {
        spdlog::error("Error in parsing {0}", m_cfg_name);
        throw std::exception();
    }
    
    
    if (m_doc.HasMember("application")) {
        m_app = get_string("application");
        set_app(m_app);
    } else {
        spdlog::error("Error in finding application in config file {0}.", m_cfg_name);
        throw std::exception();
    }
}

void Config::set_app(std::string app)
{
    set_current(app);

    auto sz = get_obj("window");
    if (sz->IsArray()) {
        m_width = ((*sz)[0]).GetInt();
        m_height = (*sz)[1].GetInt();
    }
    sz = get_obj("rt");
    if (sz->IsArray()) {
        m_rt_width = ((*sz)[0]).GetInt();
        m_rt_height = (*sz)[1].GetInt();
    }


    spdlog::info("Reading {3} for {0} with screen {1}x{2}.", m_app, m_width, m_height, app);
}

bool Config::check_model(std::string attrib)
{
    return false;
}

Config::~Config()
{
    //m_doc.Clear();
}

rapidjson::Document::ValueType* Config::get_obj(std::string key, std::string prefix)
{
    using namespace rapidjson;

    auto find_obj = [this](std::string key) {
        Document::ValueType* obj = &m_doc;
        std::vector<std::string> r = split<std::vector<std::string>>(key, '/');

        for (auto& ak : r) {
            if (ak == "") {
                continue;
            } else if (obj->HasMember(ak.c_str())) {
                obj = &(*obj)[ak.c_str()];
            } else {
                obj = nullptr;
                break;
            }
        }
        return obj;
    };

    //Always take global first priority
    std::string f_key = (prefix == "") ? (m_app + "/" + key) : (prefix + "/" + key);

    auto g_obj = find_obj(key);
    auto obj = g_obj ? g_obj : find_obj(f_key);
    if (obj == nullptr) {
        spdlog::warn("{0} or {1} not exist in config", key, f_key);
    }
    return obj;

}

bool Config::get_bool(std::string key, std::string prefix)
{
    auto v = get_obj(key, prefix);
    if (v) {
        return v->GetBool();
    } else {
        spdlog::warn("{0} doesn't exist, return false instead", key);
        return false;
    }
}

int Config::get_int(std::string key, std::string prefix)
{
    auto v = get_obj(key, prefix);
    if (v) {
        return v->GetInt();
    } else {
        spdlog::warn("{0} doesn't exist, return 0 instead", key);
        return 0;
    }
}

int Config::get_uint(std::string key, std::string prefix)
{
    auto v = get_obj(key, prefix);
    if (v) {
        return v->GetUint();
    } else {
        spdlog::warn("{0} doesn't exist, return 0 instead", key);
        return 0;
    }
}

std::string Config::get_string(std::string key, std::string prefix) 
{
    auto v = get_obj(key, prefix);
    return v->GetString();

}

std::vector<std::string> Config::get_object_keys(std::string key, std::string prefix) 
{
    std::vector<std::string> result;
    auto v = get_obj(key, prefix);
    if (v->IsObject()) {
        for (auto o = v->MemberBegin(); o != v->MemberEnd(); o++) {
            result.push_back(std::string(o->name.GetString()));
        }
    }
    
    return result;

}

std::unordered_map<std::string, bool> Config::get_object_settings(std::string key, std::string prefix)
{
    std::unordered_map<std::string, bool> result;
    auto v = get_obj(key, prefix);
    if (v->IsObject()) {
        for (auto o = v->MemberBegin(); o != v->MemberEnd(); o++) {
            if (o->value.IsBool()) {
                result[o->name.GetString()] = o->value.GetBool();

            }
        }
    }
    
    return result;

}
