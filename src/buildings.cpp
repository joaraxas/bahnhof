#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/math.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/resources/storage.h"
#include "bahnhof/buildings/buildings.h"

Building::Building(Game* whatgame, int x, int y, int w, int h, 
					std::set<resourcetype> need, 
					std::set<resourcetype> production)
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
	if(storage){
		for(auto resource : wants)
			storage->setaccepting(resource, true);
		for(auto resource : makes)
			storage->setproviding(resource, true);
	}
	color = {0,0,0,255};
}

void Building::update(int ms)
{
	timeleft = fmax(0,timeleft-ms);
	if(storage)
	if(timeleft==0){
		int got = 0;
		for(auto want: wants)
			got += storage->unloadstorage(want, 1);
		if(wants.size()==0)
			got = 1;
		if(got > 0){
			if(!makes.empty()){
				for(auto product: makes)
					storage->loadstorage(product, got);
			}
			else{
				game->getgamestate().money+=got;
			}
		}
		timeleft = 10000;
	}
}

void Building::render(Rendering* r)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	r->renderfilledrectangle(rect);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

Brewery::Brewery(Game* game, Vec pos) : Building(game, pos.x, pos.y, 100, 50, {hops, barley}, {beer})
{
	color = {63, 63, 127, 255};
}

Hopsfield::Hopsfield(Game* game, Vec pos) : Building(game, pos.x, pos.y, 200, 200, {}, {hops})
{
	color = {63, 127, 63, 255};
}

Barleyfield::Barleyfield(Game* game, Vec pos) : Building(game, pos.x, pos.y, 200, 200, {}, {barley})
{
	color = {127, 127, 31, 255};
}

City::City(Game* game, Vec pos) : Building(game, pos.x, pos.y, 100, 150, {beer}, {})
{
	color = {63, 63, 31, 255};
}