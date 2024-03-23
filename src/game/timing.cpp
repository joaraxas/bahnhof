#include<SDL.h>
#include "bahnhof/common/timing.h"

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