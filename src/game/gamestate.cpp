#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/rollingstock/rollingstockmanager.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/buildings/buildingmanager.h"
#include "bahnhof/resources/storage.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/shape.h"


Gamestate::Gamestate(Game* whatgame)
{
	game = whatgame;
	money = 600;
	newwagonstate = State(1, 0.2, true);
	inittracks();
	routing = std::make_unique<RouteManager>(tracksystem.get());
	rollingstockmanager = std::make_unique<RollingStockManager>(game);
	trainmanager = std::make_unique<TrainManager>(tracksystem.get(), *rollingstockmanager);
	buildingmanager = std::make_unique<BuildingManager>(game);
	trainmanager->inittrain(State(1,0.8,1));
}

Gamestate::~Gamestate()
{
}

void Gamestate::update(int ms)
{
	int lastmoney = money;

	Tracks::Signaling::update(*tracksystem, ms);
	trainmanager->update(ms);
	buildingmanager->update(ms);

	revenue += money-lastmoney;

	time += ms;
}

void Gamestate::randommap()
{
	for(int i=0; i<6; i++){
		Vec newpos = randpos(100,50);
		Vec size = buildingmanager->gettypefromid(brewery).size;
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-size.x*0.5-randint(storageextraw);
		int storagey = newpos.y-size.y*0.5-randint(storageextrah);
		storages.emplace_back(new Storage(game, storagex, storagey, storageextraw+400, storageextrah+400));
		buildingmanager->addbuilding(std::make_unique<Brewery>(game, std::make_unique<Rectangle>(newpos, size.x, size.y)));
	}
	for(int i=0; i<4; i++){
		Vec newpos = randpos(200,200);
		Vec size = buildingmanager->gettypefromid(hopsfield).size;
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-size.x*0.5-randint(storageextraw);
		int storagey = newpos.y-size.y*0.5-randint(storageextrah);
		storages.emplace_back(new Storage(game, storagex, storagey, storageextraw+400, storageextrah+400));
		buildingmanager->addbuilding(std::make_unique<Hopsfield>(game, std::make_unique<Rectangle>(newpos, size.x, size.y)));
	}
	for(int i=0; i<4; i++){
		Vec newpos = randpos(200,200);
		Vec size = buildingmanager->gettypefromid(barleyfield).size;
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-size.x*0.5-randint(storageextraw);
		int storagey = newpos.y-size.y*0.5-randint(storageextrah);
		storages.emplace_back(new Storage(game, storagex, storagey, storageextraw+400, storageextrah+400));
		buildingmanager->addbuilding(std::make_unique<Barleyfield>(game, std::make_unique<Rectangle>(newpos, size.x, size.y)));
	}
	for(int i=0; i<6; i++){
		Vec newpos = randpos(100,150);
		Vec size = buildingmanager->gettypefromid(city).size;
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-size.x*0.5-randint(storageextraw);
		int storagey = newpos.y-size.y*0.5-randint(storageextrah);
		storages.emplace_back(new Storage(game, storagex, storagey, storageextraw+400, storageextrah+400));
		buildingmanager->addbuilding(std::make_unique<City>(game, std::make_unique<Rectangle>(newpos, size.x, size.y)));
	}
}

void Gamestate::inittracks()
{
	std::vector<float> xs = {600,900,1100};
	std::vector<float> ys = {300,300,400};
	tracksystem = std::make_unique<Tracks::Tracksystem>(*game, xs, ys);
}