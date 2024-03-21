#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/gamestate.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/rollingstock/rollingstock.h"

Wagon::Wagon(Tracksystem* mytracks, State trackstate, SpriteManager* allsprites, sprites::name spritename, sprites::name iconname)
{
	tracksystem = mytracks;
	//allresources = tracksystem->allresources;
	w = 50;
	state = trackstate;
	pos = tracksystem->getpos(state);
	SpriteManager* spritemanager = allsprites;
	sprite.setspritesheet(allsprites, spritename);
	icon.setspritesheet(allsprites, iconname);
}

void Wagon::travel(float pixels)
{
	state = tracksystem->travel(state, pixels*(2*alignedforward-1));	
}

void Wagon::update(int ms)
{
	pos = tracksystem->getpos(state);
	sprite.imageangle = tracksystem->getorientation(state);
}

void Wagon::render(Rendering* r)
{
	float scale = r->getscale();
	if(loadedresource!=none){
		//Resource* resource = allresources->get(loadedresource);
		//resource->render(pos);
	}
	else if(scale<0.3){
		icon.render(r, pos);
	}
	else
		sprite.render(r, pos);
}

State Wagon::frontendstate()
{
	State frontstate = state;
	return tracksystem->travel(frontstate, w/2);
}

State Wagon::backendstate()
{
	State backstate = state;
	return tracksystem->travel(backstate, (-1)*w/2);
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

Locomotive::Locomotive(Tracksystem* mytracks, State trackstate, SpriteManager* allsprites) : Wagon(mytracks, trackstate, allsprites, sprites::tankloco, sprites::icontankloco)
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

Openwagon::Openwagon(Tracksystem* mytracks, State trackstate, SpriteManager* allsprites) : Wagon(mytracks, trackstate, allsprites, sprites::openwagon, sprites::iconopenwagon)
{}

int Openwagon::loadwagon(resourcetype type, int amount)
{
	int loadedamount = 0;
	if(type==hops||type==barley)
		loadedamount = Wagon::loadwagon(type, amount);
	return loadedamount;
}

Tankwagon::Tankwagon(Tracksystem* mytracks, State trackstate, SpriteManager* allsprites) : Wagon(mytracks, trackstate, allsprites, sprites::refrigeratorcar, sprites::iconrefrigeratorcar)
{}

int Tankwagon::loadwagon(resourcetype type, int amount)
{
	int loadedamount = 0;
	if(type==beer)
		loadedamount = Wagon::loadwagon(type, amount);
	return loadedamount;
}