#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/rollingstock/rollingstock.h"
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
	initjusttrack();
	routing = new RouteManager(tracksystem.get());
	trainmanager = new TrainManager(*game);
	inittrain(State(1,0.4,1));

	addtrainstoorphans();
	
	randommap();
}

Gamestate::~Gamestate()
{
	for(int iWagon=0; iWagon<wagons.size(); iWagon++)
		delete wagons[iWagon];
	delete routing;
	delete trainmanager;
}

void Gamestate::update(int ms)
{
	Tracks::Signaling::update(*tracksystem, ms);
	trainmanager->update(ms);

	for(auto& wagon : wagons)
		wagon->update(ms);

	int lastmoney = money;
	
	for(auto& building : buildings)
		building->update(ms);

	revenue += money-lastmoney;

	time += ms;
}

void Gamestate::addtrainstoorphans()
{
	for(int iWagon=0; iWagon<wagons.size(); iWagon++){
		if(!wagons[iWagon]->train){
			trainmanager->addtrain(new Train(*tracksystem, {wagons[iWagon]}, 0));
			std::cout<<"added train automatically"<<std::endl;
		}
	}
}

void Gamestate::randommap()
{
	for(int i=0; i<6; i++){
		Vec newpos = randpos(100,50);
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-randint(storageextraw);
		int storagey = newpos.y-randint(storageextrah);
		storages.emplace_back(new Storage(game, storagex, storagey, storageextraw+400, storageextrah+400, hops, beer));
		buildings.emplace_back(new Brewery(game, newpos));
	}
	for(int i=0; i<4; i++){
		Vec newpos = randpos(200,200);
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-randint(storageextraw);
		int storagey = newpos.y-randint(storageextrah);
		storages.emplace_back(new Storage(game, storagex, storagey, storageextraw+400, storageextrah+400, none, hops));
		buildings.emplace_back(new Hopsfield(game, newpos));
	}
	for(int i=0; i<6; i++){
		Vec newpos = randpos(100,150);
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-randint(storageextraw);
		int storagey = newpos.y-randint(storageextrah);
		storages.emplace_back(new Storage(game, storagex, storagey, storageextraw+400, storageextrah+400, beer, none));
		buildings.emplace_back(new City(game, newpos));
	}
}

void Gamestate::initjusttrack()
{
	tracksystem = std::unique_ptr<Tracks::Tracksystem>(new Tracks::Tracksystem(*game, {200,700,900}, {250,250,450}));
}

void Gamestate::inittrain(State startstate)
{
	int nWagons = wagons.size();
	wagons.emplace_back(new Locomotive(tracksystem.get(), startstate));
	for(int iWagon=0; iWagon<1; iWagon++){
		State state = Tracks::travel(*tracksystem, startstate, -(53+49)/2-iWagon*49);
		wagons.emplace_back(new Openwagon(tracksystem.get(), state));
	}
	Train* newtrain = new Train(*tracksystem, std::vector<Wagon*>(wagons.begin()+nWagons, wagons.end()), 0);
	trainmanager->addtrain(newtrain);
	
	Route* loadroute = routing->addroute();
	newtrain->route = loadroute;
}