#pragma once
#include<iostream>
#include<chrono>

class Timer{
    std::string operationname;
    using clock = std::chrono::high_resolution_clock;
    using timeunits = std::chrono::microseconds;

    timeunits since() {
        return std::chrono::duration_cast<timeunits>(clock::now()-t);
    }
    std::string fmt(timeunits diff) {
        return std::to_string(diff.count()) + " us";
    }
    
    clock timer;
    clock::time_point t;
public:
    Timer(std::string s) : t{clock::now()}, operationname{s} {}
    void stop() {
        std::cout<<operationname<<": "<<fmt(since())<<std::endl;
    }
};