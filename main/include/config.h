#ifndef _CONFIG_H_
#define _CONFIG_H_

#pragma once

#include "rapidjson/document.h"
#include "rapidjson/pointer.h"

#include <string>
#include <vector>

class Config {
public:
    Config(std::string name);
    ~Config();

    int width() { return m_width; }
    int height() { return m_height;  }

    std::string get_app() { return m_app;  }
    void set_app(std::string app);

    int get_int(std::string key);
    int get_uint(std::string key);
    std::string get_string(std::string key);
    std::vector<std::string> get_object_names(std::string key);

protected:
    void set_current(std::string app)
    {
        m_app = app;
        m_current = m_config + "/" + m_app + "/";
    }


    rapidjson::Document::ValueType* get_config(std::string key);

protected:
    std::string m_cfg_name;
    
    unsigned m_width = 0;
    unsigned m_height = 0 ;

    std::string m_app = "";

    rapidjson::Document m_doc;
    std::string m_config = "config";
    std::string m_current = "";
};

#endif //_CONFIG_Hy