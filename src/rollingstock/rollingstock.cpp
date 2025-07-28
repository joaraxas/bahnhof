#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/gamestate.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/rollingstock/trainmanager.h"

Wagon::Wagon(Tracks::Tracksystem* mytracks, State trackstate, sprites::name spritename, sprites::name iconname)
{
	tracksystem = mytracks;
	state = trackstate;
	pos = getpos(*tracksystem, state);
	SpriteManager& spritemanager = tracksystem->game->getsprites();
	sprite.setspritesheet(spritemanager, spritename);
	w = sprite.getsize().x;
	icon.setspritesheet(spritemanager, iconname);
	icon.zoomed = false;
	tracksystem->references->wagons.push_back(this);
}

Wagon::~Wagon()
{
	tracksystem->references->removewagonreference(this);
}

void Wagon::travel(float pixels)
{
	state = Tracks::travel(*tracksystem, state, pixels*(2*alignedforward-1));	
}

void Wagon::update(int ms)
{
	pos = getpos(*tracksystem, state);
	sprite.imageangle = getorientation(*tracksystem, state);
	sprite.imagespeed = train->speed*0.2*(2*alignedforward-1);
	sprite.updateframe(ms);
}

void Wagon::render(Rendering* r)
{
	float scale = r->getcamscale();
	if(scale<0.3)
		icon.render(r, pos);
	else
		sprite.render(r, pos);
	if(cargo && cargo->getloadedresourcetype()!=none){
		Resource* resource = cargo->getloadedresource();
		resource->render(r, pos);
	}
}

State Wagon::frontendstate()
{
	State frontstate = state;
	return Tracks::travel(*tracksystem, frontstate, w/2);
}

State Wagon::backendstate()
{
	State backstate = state;
	return Tracks::travel(*tracksystem, backstate, (-1)*w/2);
}

std::vector<State*> Wagon::getstates()
{
	return std::vector<State*>({&state});
}

int Wagon::loadwagon(resourcetype resource, int amount)
{
	if(cargo)
		return cargo->load(resource, amount);
	return 0;
}

int Wagon::unloadwagon(resourcetype& unloadedresource)
{
	if(cargo)
		return cargo->unload(unloadedresource);
	return 0;
}

float Wagon::getpower()
{
	if(engine)
		return engine->getpower();
	return 0;
}

WagonInfo Wagon::getinfo()
{
	resourcetype loadedresource = none;
	int loadamount = 0;
	if(cargo){
		loadamount = cargo->getloadedamount();
		loadedresource = cargo->getloadedresourcetype();
	}
	WagonInfo info(icon.getname(), loadedresource, loadamount);
	return info;
}

Locomotive::Locomotive(Tracks::Tracksystem* mytracks, State trackstate) : Wagon(mytracks, trackstate, sprites::tankloco, sprites::icontankloco)
{
	engine = std::make_unique<RollingStock::Engine>(*this);
}


namespace RollingStock{

Engine::Engine(Wagon& w) : wagon(w) {w.hasdriver = true;};

float Engine::getpower()
{
	if(abs(wagon.train->speed)<maxspeed[wagon.alignedforward==wagon.train->gasisforward])
		return P[wagon.alignedforward==wagon.train->gasisforward];
	else
		return 0;
}

int Cargo::load(const resourcetype type,  const int amount){
	int loadedamount = 0;
	if(storableresources.contains(type)){
		if(loadedresource == type || loadedresource == none){
			loadedamount = fmin(amount, maxamount - loadamount);
			loadamount += loadedamount;
			if(loadedamount>0)
				loadedresource = type;
		}
	}
	return loadedamount;
}

int Cargo::unload(resourcetype& type){
	type = loadedresource;
	loadedresource = none;
	int unloadedamount = loadamount;
	loadamount = 0;
	return unloadedamount;
}

Resource* Cargo::getloadedresource(){
	return allresources.get(loadedresource);
}

}

Openwagon::Openwagon(Tracks::Tracksystem* mytracks, State trackstate) : Wagon(mytracks, trackstate, sprites::openwagon, sprites::iconopenwagon)
{
	std::unordered_set<resourcetype> s = {hops, barley};
	cargo = std::make_unique<RollingStock::Cargo>(*this, 
												tracksystem->game->getresources(), 
												s);
}

Tankwagon::Tankwagon(Tracks::Tracksystem* mytracks, State trackstate) : Wagon(mytracks, trackstate, sprites::refrigeratorcar, sprites::iconrefrigeratorcar)
{
	std::unordered_set<resourcetype> s = {beer};
	cargo = std::make_unique<RollingStock::Cargo>(*this, 
												tracksystem->game->getresources(), 
												s);
}