#include<iostream>
#include<string>
#include "bahnhof/track/track.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/routing/routing.h"

TrainManager::TrainManager(Game& newgame)
{
    tracks = &newgame.getgamestate().gettracksystems();
    std::cout<<"Train manager created"<<std::endl;
}

void TrainManager::getinput(InputManager* input, int mslogic)
{
    for(int iTrain=0; iTrain<trains.size(); iTrain++)
        trains[iTrain]->getinput(input, mslogic);
}

void TrainManager::update(int ms)
{
	for(int iTrain=0; iTrain<trains.size(); iTrain++)
		for(int jTrain=0; jTrain<trains.size(); jTrain++)
			if(iTrain!=jTrain)
				trains[iTrain]->checkcollision(ms, trains[jTrain].get());

	for(int iTrain=trains.size()-1; iTrain>=0; iTrain--)
		if(trains[iTrain]->wagons.size() == 0)
			trains.erase(trains.begin()+iTrain);

	for(int iTrain=0; iTrain<trains.size(); iTrain++)
		trains[iTrain]->update(ms);

	//for(auto& wagon : wagons)
	//	wagon->update(ms);
}

void TrainManager::render(Rendering* r)
{
	for(auto& train : trains)
		train->render(r);
}

void TrainManager::addtrain(Train* newtrain)
{
    trains.emplace_back(newtrain);
}

void TrainManager::deselectall()
{
	for(auto& train: trains){
		train->selected = false;
	}
}