#pragma once
#include<iostream>
#include<chrono>

namespace Performance{
    using namespace std::chrono;
    using clock = high_resolution_clock;

class Timer{
    microseconds since() {
        return duration_cast<microseconds>(clock::now()-t);
    }
    std::string fmt(microseconds diff) {
        return std::to_string(diff.count()) + " us";
    }
    
    std::string operationname;
    clock::time_point t;
    bool used{false};
public:
    Timer(std::string s) : t{clock::now()}, operationname{s} {restart();}
    ~Timer() {
        // thanks to this, it's enough to create a Timer object at 
        // the start of a function to measure its footprint
        if(!used)
            measure();
    }
    void restart() {
        t = clock::now();
    }
    void measure() {
        auto time = since();
        std::cout<<operationname<<": "<<'\t'<<fmt(time)<<std::endl;
        used = true;
    }
};

}