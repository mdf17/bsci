#include <fstream>
#include <sstream>
#include <iostream>

#include "Config.h"

Q_GLOBAL_STATIC(Config, config)

Config * Config::instance()
{
    return config();
}

void Config::init(std::string file)
{
    std::ifstream in(file.c_str(), std::ios::in);

    std::string line;
    while (getline(in, line)) {
        std::stringstream ss;
        ss.str(line);
        std::string key, value;
        ss >> key;
        ss >> value;
        if (m_configParams.find(key) != m_configParams.end()) {
            std::cout << "Found second instance of config option " << key << std::endl;
            continue;
        }
        m_configParams[key] = value;
    }
}

bool Config::lookup(std::string key, std::string& val)
{
    std::map<std::string, std::string>::iterator it = m_configParams.find(key);
    if (it != m_configParams.end()) {
        val = it->second;
        return true;
    }
    val = "";
    return false;
}

std::string Config::get(std::string key)
{
    return m_configParams[key];
}
