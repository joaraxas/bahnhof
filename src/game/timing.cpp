#include<iostream>
#include<SDL.h>
#include "bahnhof/common/timing.h"
#include "bahnhof/common/math.h"

TimeManager::TimeManager(){
    logspeedfactor = 0;
	ms = 0;
	mslogic = ms*speed();
	starttime = SDL_GetTicks();
	lasttime = SDL_GetTicks();
}

void TimeManager::tick(){
    ms = SDL_GetTicks() - lasttime;
    mslogic = speed()*ms;
    lasttime = SDL_GetTicks();
    float windowlen = 30.;
    fps = fps*(windowlen-1)/windowlen + 1000./ms*1./windowlen;
}
    
int TimeManager::getms(){
    return ms;
}

int TimeManager::getmslogic(){
    return mslogic;
}

void TimeManager::speedup(){
    logspeedfactor++;
}

void TimeManager::speeddown(){
    logspeedfactor--;
}

float TimeManager::speed(){
    return pow(1.5, logspeedfactor);
}
    
int TimeManager::getfps(){
    return int(fps);
}