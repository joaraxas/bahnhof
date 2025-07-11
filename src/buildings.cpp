#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/math.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/resources/storage.h"
#include "bahnhof/buildings/buildingtypes.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/track/track.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/rollingstock/rollingstock.h"


Building::Building(Game* whatgame, int x, int y, int w, int h)
{
	game = whatgame;
	rect = {x, y, w, h};
	color = {0,0,0,255};
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

Industry::Industry(Game* whatgame, int x, int y, int w, int h, 
					std::set<resourcetype> need, 
					std::set<resourcetype> production) :
					Building(whatgame, x, y, w, h)
{
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

WagonFactory::WagonFactory(Game* g, Vec pos) : Building(g, pos.x+70, pos.y-50, 400, 100)
{
	color = {0,0,0,255};
	Tracks::Tracksystem& tracksystem = game->getgamestate().gettracksystems();
	Tracks::Tracksection tracksection = Tracks::Input::buildat(tracksystem, {pos.x+400, pos.y}, {pos.x, pos.y});
	state = Tracks::travel(tracksystem, Tracks::getstartpointstate(tracksection), 200);
}

WagonFactory::WagonFactory(Game* g, nodeid node) : Building(g, 0, 0, 400, 100)
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

Brewery::Brewery(Game* game, Vec pos) : Industry(game, pos.x, pos.y, 100, 50, {hops, barley}, {beer})
{
	color = {63, 63, 127, 255};
}

Hopsfield::Hopsfield(Game* game, Vec pos) : Industry(game, pos.x, pos.y, 200, 200, {}, {hops})
{
	color = {63, 127, 63, 255};
}

Barleyfield::Barleyfield(Game* game, Vec pos) : Industry(game, pos.x, pos.y, 200, 200, {}, {barley})
{
	color = {127, 127, 31, 255};
}

City::City(Game* game, Vec pos) : Industry(game, pos.x, pos.y, 100, 150, {beer}, {})
{
	color = {63, 63, 31, 255};
}

BuildingManager::BuildingManager(Game* g) : game(g)
{
	types.push_back(BuildingType{brewery, "Brewery", sprites::beer, 120});
	types.push_back(BuildingType{hopsfield, "Hops field", sprites::hops, 20});
	types.push_back(BuildingType{barleyfield, "Barley field", sprites::barley, 70});
	types.push_back(BuildingType{wagonfactory, "Locomotive works", sprites::icontankloco, 2});
}