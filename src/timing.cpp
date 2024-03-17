#include<SDL.h>
#include "bahnhof/common/timing.h"

TimeManager::TimeManager(){
    gamespeed = 1;
	ms = 0;
	mslogic = ms*gamespeed;
	starttime = SDL_GetTicks();
	lasttime = SDL_GetTicks();
}

void TimeManager::tick(){
    ms = SDL_GetTicks() - lasttime;
    mslogic = gamespeed*ms;
    lasttime = SDL_GetTicks();
}
    
int TimeManager::getms(){
    return ms;
}

int TimeManager::getmslogic(){
    return mslogic;
}