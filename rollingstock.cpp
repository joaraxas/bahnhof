#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<string>
#include<map>
#include "utils.h"

Wagon::Wagon(Tracksystem& newtracksystem, State trackstate, std::string path)
{
	tracksystem = &newtracksystem;
	allresources = tracksystem->allresources;
	tex = loadImage(path);
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	state = trackstate;
	pos = tracksystem->getpos(state);
}

void Wagon::travel(float pixels)
{
	state = tracksystem->travel(state, pixels*(2*alignedforward-1));	
}

void Wagon::update(int ms)
{
	pos = tracksystem->getpos(state);
	imageangle = tracksystem->getorientation(state);
}

void Wagon::render()
{
	int x = int(pos.x);
	int y = int(pos.y);
	SDL_Rect srcrect = {0, 0, w, h};
	SDL_Rect rect = {int(x - w / 2), int(y - h / 2), w, h};
	rendertexture(tex, &rect, &srcrect, imageangle);
	if(loadedresource!=none){
		Resource* resource = allresources->get(loadedresource);
		resource->render(pos);
	}
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

Locomotive::Locomotive(Tracksystem& newtracksystem, State trackstate) : Wagon(newtracksystem, trackstate, "assets/loco0.png")
{
	h = h/imagenumber;
	hasdriver = true;
}

void Locomotive::update(int ms)
{
	Wagon::update(ms);
	imagespeed = train->speed*0.2*(2*alignedforward-1);
	imageindex += imagespeed*ms*0.001;
	if(imageindex>=imagenumber)
		imageindex -= imagenumber;
	if(imageindex<0)
		imageindex += imagenumber;
}

void Locomotive::render()
{
	int x = int(pos.x);
	int y = int(pos.y);
	SDL_Rect srcrect = {0, int(imageindex)*h, w, h};
	SDL_Rect rect = {int(x - w / 2), int(y - h / 2), w, h};
	rendertexture(tex, &rect, &srcrect, imageangle);
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

Openwagon::Openwagon(Tracksystem& newtracksystem, State trackstate) : Wagon(newtracksystem, trackstate, "assets/flakvagn1.png")
{}

int Openwagon::loadwagon(resourcetype type, int amount)
{
	int loadedamount = 0;
	if(type==hops||type==barley)
		loadedamount = Wagon::loadwagon(type, amount);
	return loadedamount;
}

Tankwagon::Tankwagon(Tracksystem& newtracksystem, State trackstate) : Wagon(newtracksystem, trackstate, "assets/train.png")
{}

int Tankwagon::loadwagon(resourcetype type, int amount)
{
	int loadedamount = 0;
	if(type==beer)
		loadedamount = Wagon::loadwagon(type, amount);
	return loadedamount;
}