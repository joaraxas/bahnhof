#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/math.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/resources/storage.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/buildings/buildingmanager.h"
#include "bahnhof/track/track.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/rollingstock/rollingstock.h"


Building::Building(Game* whatgame, BuildingID id, const Shape& s) : shape(s)
{
	game = whatgame;
	const BuildingType& type = game->getbuildingmanager().gettypefromid(id);
	// rect = {int(pos.x), int(pos.y), int(type.size.x), int(type.size.y)};
	color = type.color;
}

void Building::render(Rendering* r)
{
	// SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	// r->renderfilledrectangle(rect);
	SDL_Vertex verts[4];
	verts[0].position = {shape.x, shape.y};
	verts[1].position = {shape.x+shape.w, shape.y};
	verts[2].position = {shape.x+shape.w, shape.y+shape.h};
	verts[3].position = {shape.x, shape.y+shape.h};
	for (int i = 0; i < 4; ++i) {
        verts[i].color = color;
        verts[i].tex_coord = {0, 0}; // No texture
    }
	int indices[6] = {0, 1, 2, 0, 2, 3};

	SDL_RenderGeometry(renderer, NULL, verts, 4, indices, 6);
	// SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

void Building::update(int ms)
{
	timeleft = fmax(0,timeleft-ms);
	if(timeleft==0){
		trigger();
		timeleft = 10000;
	}
}

bool Building::checkclick(Vec pos)
{
	// if(pos.x>=rect.x && pos.x<=rect.x+rect.w && pos.y>=rect.y && pos.y<=rect.y+rect.h)
	// 	return true;
	return false;
}

Industry::Industry(Game* whatgame, BuildingID id, Shape s, 
					std::set<resourcetype> need, 
					std::set<resourcetype> production) :
					Building(whatgame, id, s)
{
	storage = getstorageatpoint(s.mid());
	// if(!storage)
	// storage = getstorageatpoint(Vec(x+w,y));
	// if(!storage)
	// storage = getstorageatpoint(Vec(x,y+h));
	// if(!storage)
	// storage = getstorageatpoint(Vec(x+w,y+h));
	wants = need;
	makes = production;
	if(storage){
		for(auto resource : wants)
			storage->setaccepting(resource, true);
		for(auto resource : makes)
			storage->setproviding(resource, true);
	}
}

void Industry::trigger()
{
	if(storage){
		int got = 0;
		for(auto want: wants)
			got += storage->unloadstorage(want, 1);
		if(wants.size()==0)
			got = 1;
		if(got > 0){
			if(!makes.empty()){
				for(auto product: makes)
					storage->loadstorage(product, got);
			}
			else{
				game->getgamestate().money+=got;
			}
		}
	}
}

WagonFactory::WagonFactory(Game* g, Shape s) : Building(g, wagonfactory, s)
{
	Tracks::Tracksystem& tracksystem = game->getgamestate().gettracksystems();
	Tracks::Tracksection tracksection = Tracks::Input::buildat(tracksystem, {s.x+400, s.y}, {s.x, s.y});
	state = Tracks::travel(tracksystem, Tracks::getstartpointstate(tracksection), 200);
}

WagonFactory::WagonFactory(Game* g, Shape s, nodeid node) : Building(g, wagonfactory, s)
{
	Tracks::Tracksystem& tracksystem = game->getgamestate().gettracksystems();
	Tracks::Tracksection tracksection = Tracks::Input::buildat(tracksystem, tracksystem.getnode(node), 400);
	state = Tracks::travel(tracksystem, Tracks::getstartpointstate(tracksection), 200);
}

void WagonFactory::trigger()
{
	TrainManager& trainmanager = game->getgamestate().gettrainmanager();
	Tracks::Tracksystem& tracksystem = game->getgamestate().gettracksystems();
	trainmanager.addwagon(new Openwagon(&tracksystem, state));
	state = Tracks::travel(tracksystem, state, 60);
	trainmanager.addtrainstoorphans();
}

Brewery::Brewery(Game* game, Shape s) : Industry(game, brewery, s, {hops, barley}, {beer})
{}

Hopsfield::Hopsfield(Game* game, Shape s) : Industry(game, hopsfield, s, {}, {hops})
{}

Barleyfield::Barleyfield(Game* game, Shape s) : Industry(game, barleyfield, s, {}, {barley})
{}

City::City(Game* game, Shape s) : Industry(game, city, s, {beer}, {})
{}

BuildingManager::BuildingManager(Game* g) : game(g)
{
	types[brewery] = BuildingType{brewery, "Brewery", Vec(100,50), {63,63,127,255}, sprites::beer, 120};
	types[hopsfield] = BuildingType{hopsfield, "Hops field", Vec(100,100), {63,127,63,255}, sprites::hops, 20};
	types[barleyfield] = BuildingType{barleyfield, "Barley field", Vec(100,100), {127,127,31,255}, sprites::barley, 70};
	types[city] = BuildingType{city, "City", Vec(50,100), {63,63,31,255}, sprites::iconopenwagon, 70};
	types[wagonfactory] = BuildingType{wagonfactory, "Locomotive works", Vec(400,100), {127,127,127,255}, sprites::icontankloco, 2};

	for(auto type: types){
		availabletypes.push_back(type.second);
	}
}