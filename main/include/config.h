#ifndef _CONFIG_H_
#define _CONFIG_H_

#pragma once

#include "rapidjson/document.h"
#include "rapidjson/pointer.h"

#include <string>
#include <vector>
#include <unordered_map>

class Config {
public:
    Config(std::string name);
    ~Config();

    int width() { return m_width; }
    int height() { return m_height;  }

    int rt_width() { return m_rt_width; }
    int rt_height() { return m_rt_height;  }

    std::string get_app() { return m_app;  }
    void set_app(std::string app);

    bool check_model(std::string attrib);

    bool get_bool(std::string key, std::string prefix = "");
    int get_int(std::string key, std::string prefix = "");
    int get_uint(std::string key, std::string prefix = "");
    std::string get_string(std::string key, std::string prefix = "");


    std::vector<std::string> get_object_keys(std::string key, std::string prefix = "");
    std::unordered_map<std::string, bool> get_object_settings(std::string key, std::string prefix = "");

protected :
    rapidjson::Document::ValueType* get_obj(std::string key, std::string prefix = "");

    void set_current(std::string app)
    {
        m_app = app;
    }

protected:
    std::string m_cfg_name;
    
    unsigned m_width = 0;
    unsigned m_height = 0 ;

    unsigned m_rt_width = 0;
    unsigned m_rt_height = 0;

    std::string m_app = "";
    rapidjson::Document m_doc;

};

#endif //_CONFIG_Hy