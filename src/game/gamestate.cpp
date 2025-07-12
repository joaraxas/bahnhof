#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/resources/storage.h"
#include "bahnhof/common/gamestate.h"


Gamestate::Gamestate(Game* whatgame)
{
	game = whatgame;
	money = 600;
	newwagonstate = State(1, 0.2, true);
	inittracks();
	routing = std::make_unique<RouteManager>(tracksystem.get());
	trainmanager = std::make_unique<TrainManager>(tracksystem.get());
	trainmanager->inittrain(State(1,0.8,1));

	randommap();
}

Gamestate::~Gamestate()
{
}

void Gamestate::update(int ms)
{
	Tracks::Signaling::update(*tracksystem, ms);
	trainmanager->update(ms);

	int lastmoney = money;
	
	for(auto& building : buildings)
		building->update(ms);

	revenue += money-lastmoney;

	time += ms;
}

void Gamestate::randommap()
{
	for(int i=0; i<6; i++){
		Vec newpos = randpos(100,50);
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-randint(storageextraw);
		int storagey = newpos.y-randint(storageextrah);
		// storages.emplace_back(new Storage(game, storagex, storagey, storageextraw+400, storageextrah+400));
		// buildings.emplace_back(new Brewery(game, newpos));
	}
	for(int i=0; i<4; i++){
		Vec newpos = randpos(200,200);
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-randint(storageextraw);
		int storagey = newpos.y-randint(storageextrah);
	// 	storages.emplace_back(new Storage(game, storagex, storagey, storageextraw+400, storageextrah+400));
	// 	buildings.emplace_back(new Hopsfield(game, newpos));
	}
	for(int i=0; i<4; i++){
		Vec newpos = randpos(200,200);
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-randint(storageextraw);
		int storagey = newpos.y-randint(storageextrah);
		// storages.emplace_back(new Storage(game, storagex, storagey, storageextraw+400, storageextrah+400));
		// buildings.emplace_back(new Barleyfield(game, newpos));
	}
	for(int i=0; i<6; i++){
		Vec newpos = randpos(100,150);
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-randint(storageextraw);
		int storagey = newpos.y-randint(storageextrah);
		// storages.emplace_back(new Storage(game, storagex, storagey, storageextraw+400, storageextrah+400));
		// buildings.emplace_back(new City(game, newpos));
	}
}

void Gamestate::inittracks()
{
	std::vector<float> xs = {600,900,1100};
	std::vector<float> ys = {300,300,400};
	tracksystem = std::make_unique<Tracks::Tracksystem>(*game, xs, ys);
}