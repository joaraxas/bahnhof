#pragma once

class TimeManager
{
public:
    TimeManager();
    void tick();
    int getms();
    int getmslogic();
    float speedfactor;
private:
    int ms;
    int mslogic;
    int starttime;
    int lasttime;
};