#pragma once
#include <string>
#include <format>

bool loadlanguage(std::string path);
const std::string& tr(std::string id);
template<typename... Args>
inline std::string tr(std::string id, Args&&... args){
    return std::vformat(tr(id), 
        std::make_format_args(args...)
    );
}