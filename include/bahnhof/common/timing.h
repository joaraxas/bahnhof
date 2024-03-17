#pragma once

class TimeManager
{
public:
    TimeManager();
    void tick();
    int getms();
    int getmslogic();
    float gamespeed;
private:
    int ms;
    int mslogic;
    int starttime;
    int lasttime;
};