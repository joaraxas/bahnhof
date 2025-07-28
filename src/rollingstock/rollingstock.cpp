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

Wagon::Wagon(Tracks::Tracksystem* mytracks, State trackstate, sprites::name spritename, sprites::name iconname) :
	axes(std::make_unique<RollingStock::Axes>(*this, *mytracks, trackstate))
{
	pos = axes->getpos();
	SpriteManager& spritemanager = mytracks->game->getsprites();
	sprite.setspritesheet(spritemanager, spritename);
	w = sprite.getsize().x;
	icon.setspritesheet(spritemanager, iconname);
	icon.zoomed = false;
}

Wagon::~Wagon()
{}

void Wagon::update(int ms)
{
	sprite.imagespeed = train->speed*0.2*(2*alignedforward-1);
	sprite.updateframe(ms);
	pos = axes->getpos();
}

void Wagon::render(Rendering* r)
{
	sprite.imageangle = axes->getorientation();
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

namespace RollingStock{

Axes::Axes(Wagon& w, Tracks::Tracksystem& t, State midstate) : wagon(w), tracks(t), state(midstate) {
	tracks.references->wagons.push_back(this);
}

Axes::~Axes(){
	tracks.references->removewagonreference(this);
}

Vec Axes::getpos(){
	return Tracks::getpos(tracks, state);
}

float Axes::getorientation(){
	return Tracks::getorientation(tracks, state);
}

void Axes::travel(float pixels){
	state = Tracks::travel(tracks, state, pixels*(2*wagon.alignedforward-1));
}

State Axes::frontendstate()
{
	State frontstate = state;
	return Tracks::travel(tracks, frontstate, wagon.w/2);
}

State Axes::backendstate()
{
	State backstate = state;
	return Tracks::travel(tracks, backstate, (-1)*wagon.w/2);
}

std::vector<State*> Axes::getstates()
{
	return std::vector<State*>({&state});
}

Engine::Engine(Wagon& w) : wagon(w) {
	wagon.hasdriver = true;
};

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

Locomotive::Locomotive(Tracks::Tracksystem* mytracks, State trackstate) : Wagon(mytracks, trackstate, sprites::tankloco, sprites::icontankloco)
{
	engine = std::make_unique<RollingStock::Engine>(*this);
}

Openwagon::Openwagon(Tracks::Tracksystem* mytracks, State trackstate) : Wagon(mytracks, trackstate, sprites::openwagon, sprites::iconopenwagon)
{
	std::unordered_set<resourcetype> s = {hops, barley};
	cargo = std::make_unique<RollingStock::Cargo>(*this, 
												mytracks->game->getresources(), 
												s);
}

Tankwagon::Tankwagon(Tracks::Tracksystem* mytracks, State trackstate) : Wagon(mytracks, trackstate, sprites::refrigeratorcar, sprites::iconrefrigeratorcar)
{
	std::unordered_set<resourcetype> s = {beer};
	cargo = std::make_unique<RollingStock::Cargo>(*this, 
												mytracks->game->getresources(), 
												s);
}