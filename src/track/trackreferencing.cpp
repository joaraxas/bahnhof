#include<iostream>
#include<string>
#include<map>
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/track/trackinternal.h"
#include "bahnhof/track/track.h"


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
}


}