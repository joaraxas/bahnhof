#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<string>
#include<map>
#include "utils.h"

std::vector<std::unique_ptr<Building>> buildings;

Building::Building(ResourceManager& resources, int x, int y, int w, int h, resourcetype need, resourcetype production)
{
	allresources = &resources;
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
		int got = storage->unloadstorage(wants, 1);
		if(wants==none) got = 1;
		int put = storage->loadstorage(makes, got);
		if(makes==none && got){
			money+=got; 
			//std::cout << money << std::endl;
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

Brewery::Brewery(ResourceManager& resources, int x, int y, int w, int h) : Building(resources, x, y, w, h, hops, beer)
{};

void Brewery::render()
{
	SDL_SetRenderDrawColor(renderer, 63, 63, 127, 255);
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

Hopsfield::Hopsfield(ResourceManager& resources, int x, int y, int w, int h) : Building(resources, x, y, w, h, none, hops)
{};

City::City(ResourceManager& resources, int x, int y, int w, int h) : Building(resources, x, y, w, h, beer, none)
{};