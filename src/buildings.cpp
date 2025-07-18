#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/math.h"
#include "bahnhof/common/shape.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/resources/storage.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/buildings/buildingmanager.h"
#include "bahnhof/track/track.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/rollingstock/rollingstock.h"


Building::Building(Game* whatgame, BuildingID id, std::unique_ptr<Shape> s) : shape(std::move(s))
{
	game = whatgame;
	const BuildingType& type = game->getgamestate().getbuildingmanager().gettypefromid(id);
	color = type.color;
}

Building::~Building()
{}

void Building::render(Rendering* r)
{
	shape->renderfilled(r, color);
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
	// if(pos.x>=rect.x && pos.x<=rect.x+rect.w && pos.y>=rect.y && pos.y<=rect.y+rect.h)
	// 	return true;
	return false;
}

Industry::Industry(Game* whatgame, BuildingID id, std::unique_ptr<Shape> s, 
					std::set<resourcetype> need, 
					std::set<resourcetype> production) :
					Building(whatgame, id, std::move(s))
{
	storage = getstorageatpoint(shape->mid());
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

WagonFactory::WagonFactory(Game* g, std::unique_ptr<Shape> s, State st) : Building(g, wagonfactory, std::move(s)), state(st)
{
	Tracks::Tracksystem& tracksystem = game->getgamestate().gettracksystems();
	tracksystem.references->buildings.push_back(this);
}

void WagonFactory::trigger()
{
	TrainManager& trainmanager = game->getgamestate().gettrainmanager();
	Tracks::Tracksystem& tracksystem = game->getgamestate().gettracksystems();
	trainmanager.addwagon(new Openwagon(&tracksystem, state));
	state = Tracks::travel(tracksystem, state, 60);
	trainmanager.addtrainstoorphans();
}

Brewery::Brewery(Game* game, std::unique_ptr<Shape> s) : Industry(game, brewery, std::move(s), {hops, barley}, {beer})
{}

Hopsfield::Hopsfield(Game* game, std::unique_ptr<Shape> s) : Industry(game, hopsfield, std::move(s), {}, {hops})
{}

Barleyfield::Barleyfield(Game* game, std::unique_ptr<Shape> s) : Industry(game, barleyfield, std::move(s), {}, {barley})
{}

City::City(Game* game, std::unique_ptr<Shape> s) : Industry(game, city, std::move(s), {beer}, {})
{}

BuildingManager::BuildingManager(Game* g) : game(g)
{
	types[brewery] = BuildingType{brewery, "Brewery", Vec(100,50), {63,63,127,255}, sprites::beer, 120};
	types[hopsfield] = BuildingType{hopsfield, "Hops field", Vec(100,100), {63,127,63,255}, sprites::hops, 20};
	types[barleyfield] = BuildingType{barleyfield, "Barley field", Vec(100,100), {127,127,31,255}, sprites::barley, 70};
	types[city] = BuildingType{city, "City", Vec(50,100), {63,63,31,255}, sprites::iconopenwagon, 70};
	types[wagonfactory] = BuildingType{wagonfactory, "Locomotive works", Vec(400,100), {127,127,127,255}, sprites::icontankloco, 2};

	for(auto type: types){
		availabletypes.push_back(type.second);
	}
}

void BuildingManager::update(int ms)
{
	for(auto& building : buildings)
		building->update(ms);
}

void BuildingManager::render(Rendering* r)
{
	for(auto& building : buildings)
		building->render(r);
}