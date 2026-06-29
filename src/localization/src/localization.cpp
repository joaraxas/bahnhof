#include <fstream>
#include <unordered_map>
#include <iostream>
#include "localization/localization.h"

class Localization
{
public:
    bool load(std::string path);
    const std::string& get(std::string id);
private:
    std::unordered_map<std::string, std::string> strings;
};

bool Localization::load(std::string path)
{
    std::ifstream file{path};
    if(!file){
        std::cerr<<"Failed to load language from path "<<path<<std::endl;
        return false;
    }
    std::string line;
    strings.clear();
    
    while(std::getline(file, line)){
        auto pos = line.find('=');
        if(pos == std::string::npos)
            continue;
        std::string key = line.substr(0, pos);
        std::string sentence = line.substr(pos+1);
        if(strings.contains(key))
            std::cout<<"warning: key \""<<key<<"\" duplicated in language, overwriting \""<<strings[key]<<"\" with \""<<sentence<<"\""<<std::endl;
        strings[key] = std::move(sentence);
    }

    return true;
}

const std::string& Localization::get(std::string id)
{
    if(strings.contains(id))
        return strings[id];
    static std::string missing = "missing: "+id;
    return missing;
}

static Localization loc{};

const std::string& tr(std::string id){
    return loc.get(id);
}

bool loadlanguage(std::string path){
    return loc.load(path);
}