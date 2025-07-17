#include<iostream>
#include<string>
#include<map>
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/track/trackinternal.h"
#include "bahnhof/track/track.h"
#include "bahnhof/buildings/buildings.h"


namespace Tracks{

void Referencehandler::removetrackorderreference(Gotostate* order)
{
	auto it = find(trackorders.begin(), trackorders.end(), order);
	if(it!=trackorders.end())
		trackorders.erase(it);
	else
		std::cout<<"Warning: tried to erase non-existing reference to Gotostate order"<<std::endl;
}

void Referencehandler::removesignalorderreference(Setsignal* order)
{
	auto it = find(signalorders.begin(), signalorders.end(), order);
	if(it!=signalorders.end())
		signalorders.erase(it);
	else
		std::cout<<"Warning: tried to erase non-existing reference to Setsignal order"<<std::endl;
}

void Referencehandler::removeswitchorderreference(Setswitch* order)
{
	auto it = find(switchorders.begin(), switchorders.end(), order);
	if(it!=switchorders.end())
		switchorders.erase(it);
	else
		std::cout<<"Warning: tried to erase non-existing reference to Setswitch order"<<std::endl;
}

void Referencehandler::removewagonreference(Wagon* wagon)
{
	auto it = find(wagons.begin(), wagons.end(), wagon);
	if(it!=wagons.end())
		wagons.erase(it);
	else
		std::cout<<"Warning: tried to erase non-existing reference to wagon"<<std::endl;
}

void Referencehandler::removebuildingreference(WagonFactory* building)
{
	auto it = find(buildings.begin(), buildings.end(), building);
	if(it!=buildings.end())
		buildings.erase(it);
	else
		std::cout<<"Warning: tried to erase non-existing reference to building"<<std::endl;
}

void Referencehandler::validatereferences()
{
	for(auto order: trackorders){
		trackid track = order->state.track;
		if(!tracksystem->gettrack(track)){
			order->invalidate();
			removetrackorderreference(order);
		}
	}
	for(auto order: signalorders){
		if(!tracksystem->getsignal(order->signal)){
			order->invalidate();
			removesignalorderreference(order);
		}
	}
	for(auto order: switchorders){
		if(!tracksystem->getswitch(order->_switch)){
			order->invalidate();
			removeswitchorderreference(order);
		}
	}
	// TODO: We will need one for buildings now as well
}

void Referencehandler::movereferenceswhentracksplits(State splitstate, Track& newtrack1, Track& newtrack2)
{
	Track* oldtrack = tracksystem->gettrack(splitstate.track);
	oldtrack->split(newtrack1, newtrack2, splitstate);
	for(auto item: trackorders){
		if(item->state.track == splitstate.track){
			item->state = oldtrack->getnewstateaftersplit(newtrack1, newtrack2, splitstate, item->state);
		}
	}
	for(auto wagon: wagons){
		for(State* stateptr: wagon->getstates()){
			if(stateptr->track == splitstate.track)
				*stateptr = oldtrack->getnewstateaftersplit(newtrack1, newtrack2, splitstate, *stateptr);
		}
	}
	for(auto item: buildings){
		if(item->getstate().track == splitstate.track){
			std::cout<<item->getstate().track <<std::endl;
			item->getstate() = oldtrack->getnewstateaftersplit(newtrack1, newtrack2, splitstate, item->getstate());
			std::cout<<item->getstate().track <<std::endl;
		}
	}
}


}