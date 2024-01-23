#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<string>
#include<map>
#include "utils.h"

Building::Building(int x, int y, int w, int h, Resource* need, Resource* production)
{
	rect = {x, y, w, h};
	storage = getstorageatpoint(Vec(x,y));
	if(!storage)
	storage = getstorageatpoint(Vec(x+w,y));
	if(!storage)
	storage = getstorageatpoint(Vec(x,y+h));
	if(!storage)
	storage = getstorageatpoint(Vec(x+w,y+h));
	wants = need;
	makes = production;
}

void Building::update(int ms)
{
	timeleft = fmax(0,timeleft-ms);
	if(timeleft==0){
		int got = storage->unloadstorage(*wants, 1);
		if(wants==nullptr) got = 1;
		int put = storage->loadstorage(*makes, got);
		if(makes==nullptr && got){
			money+=got; 
			std::cout << money << std::endl;
		}
		timeleft = 3000;
	}
}

void Building::render()
{
	SDL_SetRenderDrawColor(renderer, 63, 127, 63, 255);
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

Brewery::Brewery(int x, int y, int w, int h) : Building(x, y, w, h, nullptr, nullptr)
{
};