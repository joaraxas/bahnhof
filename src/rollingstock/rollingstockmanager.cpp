#include <map>
#include <localization/localization.h>
#include "bahnhof/common/math.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/rollingstock/rollingstockmanager.h"


RollingStockManager::RollingStockManager(Game* g) : game(g)
{
	WagonType* wagon = &types[WagonID::tanklocomotive];
	wagon->id = WagonID::tanklocomotive;
	wagon->name = tr("rollingstock.tankloco");
	wagon->iconname = sprites::icontankloco;
	wagon->spritename = sprites::tankloco;
	wagon->cost = 20_Fr;
	wagon->engine = enginetype::steam;
	wagon->forwardpower = 0.2;
	wagon->backwardpower = 0.2;
	wagon->forwardmaxspeed = 180;
	wagon->backwardmaxspeed = 90;
	
	wagon = &types[WagonID::openwagon];
	wagon->id = WagonID::openwagon;
	wagon->name = "Open wagon";
	wagon->name = tr("rollingstock.openwagon");
	wagon->iconname = sprites::iconopenwagon;
	wagon->spritename = sprites::openwagon;
	wagon->cost = 4_Fr;
	wagon->storableresources = {hops, barley};
	wagon->storagecapacity = 1;
	wagon->renderstorage = true;

	wagon = &types[WagonID::refrigeratorcar];
	wagon->id = WagonID::refrigeratorcar;
	wagon->name = tr("rollingstock.refrigeratorcar");
	wagon->iconname = sprites::iconrefrigeratorcar;
	wagon->spritename = sprites::refrigeratorcar;
	wagon->cost = 6_Fr;
	wagon->storableresources = {beer};
	wagon->storagecapacity = 1;

	for(auto& [id, type] : types){
		availabletypes.push_back(&type);
	}
}
