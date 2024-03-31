#include<iostream>
#include<string>
#include<map>
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/track/trackinternal.h"
#include "bahnhof/track/track.h"


namespace Tracks{
void Referencehandler::removeorderreference(Gotostate* order)
{
	auto it = find(trackorders.begin(), trackorders.end(), order);
	if(it!=trackorders.end())
		trackorders.erase(it);
	else
		std::cout<<"Warning: tried to erase non-existing reference to order"<<std::endl;
}

void Referencehandler::removewagonreference(Wagon* wagon)
{
	auto it = find(wagons.begin(), wagons.end(), wagon);
	if(it!=wagons.end())
		wagons.erase(it);
	else
		std::cout<<"Warning: tried to erase non-existing reference to wagon"<<std::endl;
}


}