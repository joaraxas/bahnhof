#include<iostream>
#include<string>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/routing/routing.h"

TrainManager::TrainManager(Tracks::Tracksystem* newtracks)
{
    tracks = newtracks;
    std::cout<<"Train manager created"<<std::endl;
	addtrainstoorphans();
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

	for(auto& wagon : wagons)
		wagon->update(ms);
}

void TrainManager::render(Rendering* r)
{
	for(auto& wagon : wagons)
		wagon->render(r);
	for(auto& train : trains)
		train->render(r);
}

void TrainManager::addwagon(Wagon* newwagon)
{
    wagons.emplace_back(newwagon);
}

void TrainManager::addtrain(Train* newtrain)
{
    trains.emplace_back(newtrain);
}

void TrainManager::addtrainstoorphans()
{
	for(int iWagon=0; iWagon<wagons.size(); iWagon++){
		if(!wagons[iWagon]->train){
			addtrain(new Train(*tracks, {wagons[iWagon].get()}, 0));
			std::cout<<"added train automatically"<<std::endl;
		}
	}
}

void TrainManager::deselectall()
{
	for(auto& train: trains){
		train->selected = false;
	}
}

Train* TrainManager::gettrainatpos(Vec pos)
{
    Train* clickedtrain = nullptr;
    for(auto& wagon : wagons){
        if(norm(pos-wagon->pos)<wagon->w/2/tracks->game->getrendering().getscale()){
            clickedtrain = wagon->train;
        }
        if(clickedtrain) break;
    }
    return clickedtrain;
}

void TrainManager::inittrain(State startstate)
{
	int nWagons = wagons.size();
	addwagon(new Locomotive(tracks, startstate));
	for(int iWagon=0; iWagon<3; iWagon++){
		State state = Tracks::travel(*tracks, startstate, -(53+49)/2-iWagon*49);
		addwagon(new Openwagon(tracks, state));
	}
    std::vector<Wagon*> newwagonset;
    for(auto it = wagons.begin()+nWagons; it!=wagons.end(); ++it){
        newwagonset.push_back(it->get());
    }
	Train* newtrain = new Train(*tracks, newwagonset, 0);
	addtrain(newtrain);
}