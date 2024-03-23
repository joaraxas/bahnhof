#include<SDL.h>
#include "bahnhof/common/timing.h"

TimeManager::TimeManager(){
    speedfactor = 1;
	ms = 0;
	mslogic = ms*speedfactor;
	starttime = SDL_GetTicks();
	lasttime = SDL_GetTicks();
}

void TimeManager::tick(){
    ms = SDL_GetTicks() - lasttime;
    mslogic = speedfactor*ms;
    lasttime = SDL_GetTicks();
}
    
int TimeManager::getms(){
    return ms;
}

int TimeManager::getmslogic(){
    return mslogic;
}