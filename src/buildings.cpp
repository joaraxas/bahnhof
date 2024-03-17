#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/rendering.h"
#include "bahnhof/resources/storage.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/rollingstock/rollingstock.h"

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
	color = {0,0,0,255};
}

void Building::update(int ms)
{
	timeleft = fmax(0,timeleft-ms);
	if(storage)
	if(timeleft==0){
		int got = storage->unloadstorage(wants, 1);
		if(wants==none) got = 1;
		if(got){
			if(makes!=none){
				int put = storage->loadstorage(makes, got);
				storage->loadstorage(wants, got-put);
			}
			else{
				money+=got;
			}
		}
		timeleft = 10000;
	}
}

void Building::render(Rendering* rendering)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_Rect drawrect = rect;
	rendering->renderfilledrectangle(&drawrect);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

Brewery::Brewery(ResourceManager& resources, Vec pos) : Building(resources, pos.x, pos.y, 100, 50, hops, beer)
{
	color = {63, 63, 127, 255};
}

Hopsfield::Hopsfield(ResourceManager& resources, Vec pos) : Building(resources, pos.x, pos.y, 200, 200, none, hops)
{
	color = {63, 127, 63, 255};
}

City::City(ResourceManager& resources, Vec pos) : Building(resources, pos.x, pos.y, 100, 150, beer, none)
{
	color = {63, 63, 31, 255};
}