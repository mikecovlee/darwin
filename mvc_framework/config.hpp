#pragma once
#include <fstream>
#include <string>
#include <unordered_map>
namespace darwin{
    class config final{
        std::ifstream infs;
        std::unordered_map<std::string,std::string> data;
    public:
        config()=delete;
        config(const std::string& path):infs(path){}
        bool usable() const
        {
            return infs.is_open();
        }
        void read()
        {
            std::string str;
            while(std::getline(infs,str))
                data[str.substr(0,str.find('='))]=str.substr(str.find('=')+1);
        }
        const std::string& get_value(const std::string& key)
        {
            return data.at(key);
        }
    };
}