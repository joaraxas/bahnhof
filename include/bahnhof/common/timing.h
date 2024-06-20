#pragma once

class TimeManager
{
public:
    TimeManager();
    void tick();
    int getms();
    int getmslogic();
    void speedup();
    void speeddown();
    float speed();
    int getfps();
private:
    int ms;
    int mslogic;
    int starttime;
    int lasttime;
    int logspeedfactor;
    float fps=0;
};