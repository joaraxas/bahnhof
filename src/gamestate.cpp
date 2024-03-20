#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/resources/storage.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/input.h"
#include "bahnhof/common/camera.h"
#include "bahnhof/common/timing.h"


Game::Game()
{
	timer = new TimeManager();
	input = new InputManager(this);
	cam = new Camera();
	rendering = new Rendering(this, cam);
	resources = new ResourceManager();
	gamestate = new Gamestate(this);
	quit = false;
}

Game::~Game()
{
	delete timer;
	delete input;
	delete cam;
	delete rendering;
	delete gamestate;
	delete resources;
}

void Game::play()
{
	while(!quit){
		timer->tick();
		input->handle(timer->getms(), timer->getmslogic());
		gamestate->update(timer->getmslogic());
		rendering->render(gamestate);
	}
}

void Game::exit()
{
	quit = true;
}

Gamestate::Gamestate(Game* whatgame)
{
	game = whatgame;
	money = 600;
	newwagonstate = State(1, 0.2, true);
	initjusttrack();
	routing = new RouteManager(tracksystem.get());
	inittrain(State(1,0.4,1));

	addtrainstoorphans();
	
	randommap();
}

Gamestate::~Gamestate()
{
	for(int iWagon=0; iWagon<wagons.size(); iWagon++)
		delete wagons[iWagon];
	delete routing;
}

void Gamestate::update(int ms)
{
	tracksystem->update(ms);

	for(int iTrain=0; iTrain<trains.size(); iTrain++)
		for(int jTrain=0; jTrain<trains.size(); jTrain++)
			if(iTrain!=jTrain)
				trains[iTrain]->checkcollision(ms, trains[jTrain].get());

	for(int iTrain=trains.size()-1; iTrain>=0; iTrain--)
		if(trains[iTrain]->wagons.size() == 0)
			trains.erase(trains.begin()+iTrain);

	for(int iTrain=0; iTrain<trains.size(); iTrain++)
		trains[iTrain]->update(ms);

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
		storages.emplace_back(new Storage(game->resources, storagex, storagey, storageextraw+400, storageextrah+400, hops, beer));
		buildings.emplace_back(new Brewery(game->resources, newpos));
	}
	for(int i=0; i<4; i++){
		Vec newpos = randpos(200,200);
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-randint(storageextraw);
		int storagey = newpos.y-randint(storageextrah);
		storages.emplace_back(new Storage(game->resources, storagex, storagey, storageextraw+400, storageextrah+400, none, hops));
		buildings.emplace_back(new Hopsfield(game->resources, newpos));
	}
	for(int i=0; i<6; i++){
		Vec newpos = randpos(100,150);
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-randint(storageextraw);
		int storagey = newpos.y-randint(storageextrah);
		storages.emplace_back(new Storage(game->resources, storagex, storagey, storageextraw+400, storageextrah+400, beer, none));
		buildings.emplace_back(new City(game->resources, newpos));
	}
}

void Gamestate::initjusttrack()
{
	tracksystem = std::unique_ptr<Tracksystem>(new Tracksystem(game, {200,700,1000}, {250,250,550}));
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
	
	Route* loadroute = routing->addroute();
	trains.back()->route = loadroute;
}