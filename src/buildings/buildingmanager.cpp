#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/math.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/buildings/buildingmanager.h"


BuildingManager::BuildingManager(Game* g) : game(g)
{
	types[brewery] = BuildingType{brewery, "Brewery", Vec(232,150), {63,63,127,255}, sprites::brewery, sprites::beer, 120};
	types[hopsfield] = BuildingType{hopsfield, "Hops field", Vec(248,168), {63,127,63,255}, sprites::hopsfield, sprites::hops, 90};
	types[barleyfield] = BuildingType{barleyfield, "Barley field", Vec(300,450), {127,127,31,255}, sprites::barleyfield, sprites::barley, 70};
	types[city] = BuildingType{city, "City", Vec(170,132), {63,63,31,255}, sprites::tavern, sprites::iconopenwagon, 200};
	types[wagonfactory] = BuildingType{wagonfactory, "Locomotive works", Vec(370,185), {127,127,127,255}, sprites::wagonfactory, sprites::icontankloco, 100};

	availabletypes.push_back(types[brewery]);
	availabletypes.push_back(types[wagonfactory]);
}

void BuildingManager::update(int ms)
{
	for(auto& building : buildings)
		building->update(ms);
}

void BuildingManager::render(Rendering* r)
{
	for(auto& building : buildings)
		building->render(r);
}

bool BuildingManager::leftclick(Vec mappos)
{
	for(auto& building : buildings){
		if(building->checkcollisionwithpoint(mappos)){
			building->leftclick(mappos);
			return true;
		}
	}
	return false;
}

void BuildingManager::addbuilding(std::unique_ptr<Building> b)
{
	buildings.emplace_back(std::move(b));
}

bool BuildingManager::checkcollision(const Shape& shape)
{
	for(auto& building : buildings){
		if(building->checkcollisionwithshape(shape))
			return true;
	}
	return false;
}