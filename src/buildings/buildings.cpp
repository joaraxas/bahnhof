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
	if(hassprite && nicetracks)
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

bool Building::checkcollisionwithpoint(Vec pos)
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
