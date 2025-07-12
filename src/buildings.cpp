#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/math.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/resources/storage.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/buildings/buildingmanager.h"
#include "bahnhof/track/track.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/rollingstock/rollingstock.h"


Building::Building(Game* whatgame, const BuildingType& type, Vec pos)
{
	game = whatgame;
	rect = {int(pos.x), int(pos.y), int(type.size.x), int(type.size.y)};
	color = type.color;
}

void Building::render(Rendering* r)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	r->renderfilledrectangle(rect);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

void Building::update(int ms)
{
	timeleft = fmax(0,timeleft-ms);
	if(timeleft==0){
		trigger();
		timeleft = 10000;
	}
}

bool Building::checkclick(Vec pos)
{
	if(pos.x>=rect.x && pos.x<=rect.x+rect.w && pos.y>=rect.y && pos.y<=rect.y+rect.h)
		return true;
	return false;
}

Industry::Industry(Game* whatgame, const BuildingType& type, Vec pos, 
					std::set<resourcetype> need, 
					std::set<resourcetype> production) :
					Building(whatgame, type, pos)
{
	storage = getstorageatpoint(pos);
	// if(!storage)
	// storage = getstorageatpoint(Vec(x+w,y));
	// if(!storage)
	// storage = getstorageatpoint(Vec(x,y+h));
	// if(!storage)
	// storage = getstorageatpoint(Vec(x+w,y+h));
	wants = need;
	makes = production;
	if(storage){
		for(auto resource : wants)
			storage->setaccepting(resource, true);
		for(auto resource : makes)
			storage->setproviding(resource, true);
	}
}

void Industry::trigger()
{
	if(storage){
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
	}
}

WagonFactory::WagonFactory(Game* g, const BuildingType& type, Vec pos) : Building(g, type, pos + Vec(70,-50))
{
	color = {0,0,0,255};
	Tracks::Tracksystem& tracksystem = game->getgamestate().gettracksystems();
	Tracks::Tracksection tracksection = Tracks::Input::buildat(tracksystem, {pos.x+400, pos.y}, {pos.x, pos.y});
	state = Tracks::travel(tracksystem, Tracks::getstartpointstate(tracksection), 200);
}

WagonFactory::WagonFactory(Game* g, const BuildingType& type, nodeid node) : Building(g, type, Vec(0,0))
{
	color = {0,0,0,255};
	Tracks::Tracksystem& tracksystem = game->getgamestate().gettracksystems();
	Tracks::Tracksection tracksection = Tracks::Input::buildat(tracksystem, tracksystem.getnode(node), 400);
	state = Tracks::travel(tracksystem, Tracks::getstartpointstate(tracksection), 200);
}

void WagonFactory::trigger()
{
	TrainManager& trainmanager = game->getgamestate().gettrainmanager();
	Tracks::Tracksystem& tracksystem = game->getgamestate().gettracksystems();
	trainmanager.addwagon(new Openwagon(&tracksystem, state));
	state = Tracks::travel(tracksystem, state, 60);
	trainmanager.addtrainstoorphans();
}

Brewery::Brewery(Game* game, const BuildingType& type, Vec pos) : Industry(game, type, pos, {hops, barley}, {beer})
{
	color = {63, 63, 127, 255};
}

Hopsfield::Hopsfield(Game* game, const BuildingType& type, Vec pos) : Industry(game, type, pos, {}, {hops})
{
	color = {63, 127, 63, 255};
}

Barleyfield::Barleyfield(Game* game, const BuildingType& type, Vec pos) : Industry(game, type, pos, {}, {barley})
{
	color = {127, 127, 31, 255};
}

City::City(Game* game, const BuildingType& type, Vec pos) : Industry(game, type, pos, {beer}, {})
{
	color = {63, 63, 31, 255};
}

BuildingManager::BuildingManager(Game* g) : game(g)
{
	types[brewery] = BuildingType{brewery, "Brewery", Vec(100,50), {100,200,150,255}, sprites::beer, 120};
	types[hopsfield] = BuildingType{hopsfield, "Hops field", Vec(100,100), {50,230,50,255}, sprites::hops, 20};
	types[barleyfield] = BuildingType{barleyfield, "Barley field", Vec(100,100), {230,230,50,255}, sprites::barley, 70};
	types[wagonfactory] = BuildingType{wagonfactory, "Locomotive works", Vec(400,100), {50,50,50,255}, sprites::icontankloco, 2};

	for(auto type: types){
		availabletypes.push_back(type.second);
	}
}