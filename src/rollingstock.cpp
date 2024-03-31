#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/gamestate.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/rollingstock/rollingstock.h"

Wagon::Wagon(Tracks::Tracksystem* mytracks, State trackstate, sprites::name spritename, sprites::name iconname)
{
	tracksystem = mytracks;
	allresources = &tracksystem->game->getresources();
	w = 50;
	state = trackstate;
	pos = getpos(*tracksystem, state);
	SpriteManager& spritemanager = tracksystem->game->getsprites();
	sprite.setspritesheet(spritemanager, spritename);
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
}

void Wagon::render(Rendering* r)
{
	float scale = r->getscale();
	if(scale<0.3)
		icon.render(r, pos);
	else
		sprite.render(r, pos);
	if(loadedresource!=none){
		Resource* resource = allresources->get(loadedresource);
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

float Wagon::getpower()
{
	return 0;
}

int Wagon::loadwagon(resourcetype resource, int amount)
{
	int loadedamount = 0;
	if(loadedresource == resource || loadedresource == none){
		loadedamount = fmin(amount, maxamount - loadamount);
		loadamount += loadedamount;
		if(loadedamount>0)
			loadedresource = resource;
	}
	return loadedamount;
}

int Wagon::unloadwagon(resourcetype* unloadedresource)
{
	*unloadedresource = loadedresource;
	loadedresource = none;
	int unloadedamount = loadamount;
	loadamount = 0;
	return unloadedamount;
}

Locomotive::Locomotive(Tracks::Tracksystem* mytracks, State trackstate) : Wagon(mytracks, trackstate, sprites::tankloco, sprites::icontankloco)
{
	hasdriver = true;
}

void Locomotive::update(int ms)
{
	Wagon::update(ms);
	sprite.imagespeed = train->speed*0.2*(2*alignedforward-1);
	sprite.updateframe(ms);
}

float Locomotive::getpower()
{
	if(abs(train->speed)<maxspeed[alignedforward==train->gasisforward])
		return P[alignedforward==train->gasisforward];
	else
		return 0;
}

int Locomotive::loadwagon(resourcetype resource, int amount)
{
	int loadedamount = 0;
	return loadedamount;
}

int Locomotive::unloadwagon(resourcetype* unloadedresource)
{
	*unloadedresource = none;
	int unloadedamount = 0;
	return unloadedamount;
}

Openwagon::Openwagon(Tracks::Tracksystem* mytracks, State trackstate) : Wagon(mytracks, trackstate, sprites::openwagon, sprites::iconopenwagon)
{}

int Openwagon::loadwagon(resourcetype type, int amount)
{
	int loadedamount = 0;
	if(type==hops||type==barley)
		loadedamount = Wagon::loadwagon(type, amount);
	return loadedamount;
}

Tankwagon::Tankwagon(Tracks::Tracksystem* mytracks, State trackstate) : Wagon(mytracks, trackstate, sprites::refrigeratorcar, sprites::iconrefrigeratorcar)
{}

int Tankwagon::loadwagon(resourcetype type, int amount)
{
	int loadedamount = 0;
	if(type==beer)
		loadedamount = Wagon::loadwagon(type, amount);
	return loadedamount;
}