#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/math.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/resources/storage.h"
#include "bahnhof/buildings/buildings.h"

Building::Building(Game* whatgame, int x, int y, int w, int h, resourcetype need, resourcetype production)
{
	game = whatgame;
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
				game->getgamestate().money+=got;
			}
		}
		timeleft = 10000;
	}
}

void Building::render(Rendering* rendering)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	rendering->renderfilledrectangle(rect);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

Brewery::Brewery(Game* game, Vec pos) : Building(game, pos.x, pos.y, 100, 50, hops, beer)
{
	color = {63, 63, 127, 255};
}

Hopsfield::Hopsfield(Game* game, Vec pos) : Building(game, pos.x, pos.y, 200, 200, none, hops)
{
	color = {63, 127, 63, 255};
}

City::City(Game* game, Vec pos) : Building(game, pos.x, pos.y, 100, 150, beer, none)
{
	color = {63, 63, 31, 255};
}