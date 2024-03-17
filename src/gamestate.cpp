#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<string>
#include<map>
#include "bahnhof/utils.h"
#include "bahnhof/track/track.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/resources/storage.h"
#include "bahnhof/common/constants.h"
#include "bahnhof/common/gamestate.h"

Gamestate::Gamestate()
{
	money = 600;
	newwagonstate = State(1, 0.2, true);
	initjusttrack();
	inittrain(State(1,0.4,1));
	trains.back()->route = routes.front().get();

	addtrainstoorphans();
	
	randommap();
}

Gamestate::~Gamestate()
{
	for(int iWagon=0; iWagon<wagons.size(); iWagon++)
		delete wagons[iWagon];
}

void Gamestate::update(int ms)
{
	time += ms;
}

void Gamestate::renderroutes()
{
	int offset = 1;
	for(auto &route : routes){
		rendertext("("+std::to_string(offset)+") "+route->name, SCREEN_WIDTH-300, (offset+1)*14, {0,0,0,0}, false);
		offset++;
	}
	rendertext("(r) Create new route", SCREEN_WIDTH-300, (offset+1)*14, {0,0,0,0}, false);
}

Route* Gamestate::addroute()
{
	Route* newroute = new Route(tracksystem.get(), "Route "+std::to_string(routes.size()+1));
	routes.emplace_back(newroute);
}

void Gamestate::addtrainstoorphans()
{
	for(int iWagon=0; iWagon<wagons.size(); iWagon++){
		if(!wagons[iWagon]->train){
			trains.emplace_back(new Train(*tracksystem, {wagons[iWagon]}, 0));
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
		storages.emplace_back(new Storage(resources, storagex, storagey, storageextraw+400, storageextrah+400, hops, beer));
		buildings.emplace_back(new Brewery(resources, newpos));
	}
	for(int i=0; i<4; i++){
		Vec newpos = randpos(200,200);
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-randint(storageextraw);
		int storagey = newpos.y-randint(storageextrah);
		storages.emplace_back(new Storage(resources, storagex, storagey, storageextraw+400, storageextrah+400, none, hops));
		buildings.emplace_back(new Hopsfield(resources, newpos));
	}
	for(int i=0; i<6; i++){
		Vec newpos = randpos(100,150);
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-randint(storageextraw);
		int storagey = newpos.y-randint(storageextrah);
		storages.emplace_back(new Storage(resources, storagex, storagey, storageextraw+400, storageextrah+400, beer, none));
		buildings.emplace_back(new City(resources, newpos));
	}
}

void Gamestate::initjusttrack()
{
	tracksystem = std::unique_ptr<Tracksystem>(new Tracksystem({200,700,1000}, {250,250,550}));
	
}

void Gamestate::inittrain(State startstate)
{
	int nWagons = wagons.size();
	wagons.emplace_back(new Locomotive(*tracksystem, startstate));
	for(int iWagon=0; iWagon<3; iWagon++){
		State state = tracksystem->travel(startstate, -(53+49)/2-iWagon*49);
		wagons.emplace_back(new Openwagon(*tracksystem, state));
	}
	trains.emplace_back(new Train(*tracksystem, std::vector<Wagon*>(wagons.begin()+nWagons, wagons.end()), 0));
	
	Route* loadroute = new Route(tracksystem.get(), "Route 1");
	routes.emplace_back(loadroute);
	trains.back()->route = loadroute;
}