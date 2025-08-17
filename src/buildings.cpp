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
#include "bahnhof/rollingstock/rollingstockmanager.h"
#include "bahnhof/ui/panels.h"


Building::Building(Game* g, BuildingID id, std::unique_ptr<Shape> s) : 
	shape(std::move(s)),
	game(g),
	type(g->getgamestate().getbuildingmanager().gettypefromid(id))
{
	color = type.color;
	panel = std::make_unique<UI::Ownership>();
	if(type.spritename){
		sprite.setspritesheet(game->getsprites(), type.spritename);
		sprite.imageangle = shape->getorientation();
		hassprite = true;
	}
}

Building::~Building()
{}

void Building::render(Rendering* r)
{
	if(hassprite)
		sprite.render(r, shape->mid());
	else
		shape->renderfilled(r, color);
}

void Building::update(int ms)
{
	timeleft = fmax(0,timeleft-ms);
	if(timeleft==0){
		trigger();
		timeleft = timebetweentriggers;
	}
}

bool Building::checkclick(Vec pos)
{
	return shape->contains(pos);
}

bool Building::leftclick(Vec pos)
{
	if(!panel->exists()){
		panel->set(new UI::BuildingPanel(&game->getui(), this));
	}
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

WagonFactory::WagonFactory(Game* g, std::unique_ptr<Shape> s, State st, RollingStockManager& r) : 
	Building(g, wagonfactory, std::move(s)), 
	state(st),
	rollingstock(r)
{
	Tracks::Tracksystem& tracksystem = game->getgamestate().gettracksystems();
	tracksystem.references->buildings.push_back(this);
	name = "Schweizerische Wagons- und Aufzügefabrik AG";
}

void WagonFactory::trigger()
{
	if(!productionqueue.empty()){
		const WagonType& type = *productionqueue.front();
		productionqueue.pop_front();

		TrainManager& trainmanager = game->getgamestate().gettrainmanager();
		Tracks::Tracksystem& tracksystem = game->getgamestate().gettracksystems();
		trainmanager.addwagon(new Wagon(&tracksystem, state, type));
		trainmanager.addtrainstoorphans(20);
	}

	if(!productionqueue.empty())
		timebetweentriggers = 3500;
	else
		timebetweentriggers = 100;
}

bool WagonFactory::leftclick(Vec pos)
{
	if(!panel->exists()){
		panel->set(new UI::FactoryPanel(&game->getui(), this));
	}
}

const std::vector<WagonType*> WagonFactory::getavailabletypes()
{
	return rollingstock.gettypes();
}

void WagonFactory::orderwagon(const WagonType& type)
{
	if(type.cost<=game->getgamestate().money){
		if(productionqueue.empty())
			timeleft = 3500;
		productionqueue.push_back(&type);
		game->getgamestate().money -= type.cost;
	}
}

const std::deque<const WagonType*>& WagonFactory::getqueue()
{
	return productionqueue;
}

Brewery::Brewery(Game* game, std::unique_ptr<Shape> s) : Industry(game, brewery, std::move(s), {hops, barley}, {beer})
{
	name = "Augustator";
}

Hopsfield::Hopsfield(Game* game, std::unique_ptr<Shape> s) : Industry(game, hopsfield, std::move(s), {}, {hops})
{}

Barleyfield::Barleyfield(Game* game, std::unique_ptr<Shape> s) : Industry(game, barleyfield, std::move(s), {}, {barley})
{}

City::City(Game* game, std::unique_ptr<Shape> s) : Industry(game, city, std::move(s), {beer}, {})
{}

BuildingManager::BuildingManager(Game* g) : game(g)
{
	types[brewery] = BuildingType{brewery, "Brewery", Vec(232,150), {63,63,127,255}, sprites::brewery, sprites::beer, 120};
	types[hopsfield] = BuildingType{hopsfield, "Hops field", Vec(248,168), {63,127,63,255}, sprites::hopsfield, sprites::hops, 20};
	types[barleyfield] = BuildingType{barleyfield, "Barley field", Vec(300,450), {127,127,31,255}, sprites::barleyfield, sprites::barley, 70};
	types[city] = BuildingType{city, "City", Vec(170,132), {63,63,31,255}, sprites::tavern, sprites::iconopenwagon, 70};
	types[wagonfactory] = BuildingType{wagonfactory, "Locomotive works", Vec(370,185), {127,127,127,255}, sprites::wagonfactory, sprites::icontankloco, 2};

	availabletypes.push_back(types[brewery]);
	availabletypes.push_back(types[wagonfactory]);
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

bool BuildingManager::leftclick(Vec mappos)
{
	for(auto& building : buildings){
		if(building->checkclick(mappos)){
			building->leftclick(mappos);
			return true;
		}
	}
	return false;
}

void BuildingManager::addbuilding(std::unique_ptr<Building> b)
{
	buildings.emplace_back(std::move(b));
}