#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/math.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/rollingstock/rollingstockmanager.h"


RollingStockManager::RollingStockManager(Game* g) : game(g)
{
	WagonType* wagon = &types[WagonID::tanklocomotive];
	wagon->id = WagonID::tanklocomotive;
	wagon->name = "Tank locomotive";
	wagon->iconname = sprites::icontankloco;
	wagon->spritename = sprites::tankloco;
	wagon->engine = enginetype::steam;
	wagon->forwardpower = 0.2;
	wagon->backwardpower = 0.2;
	wagon->forwardmaxspeed = 180;
	wagon->backwardmaxspeed = 90;
	
	wagon = &types[WagonID::openwagon];
	wagon->id = WagonID::openwagon;
	wagon->name = "Open wagon";
	wagon->iconname = sprites::iconopenwagon;
	wagon->spritename = sprites::openwagon;
	wagon->storableresources = {hops, barley};
	wagon->storagecapacity = 1;

	wagon = &types[WagonID::refrigeratorcar];
	wagon->id = WagonID::refrigeratorcar;
	wagon->name = "Refrigerator car";
	wagon->iconname = sprites::iconrefrigeratorcar;
	wagon->spritename = sprites::refrigeratorcar;
	wagon->storableresources = {beer};
	wagon->storagecapacity = 1;
}
