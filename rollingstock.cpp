#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<string>
#include<map>
#include "utils.h"

Wagon::Wagon(Tracksystem& newtracksystem, float nodediststart, std::string path)
{
	tracksystem = &newtracksystem;
	allresources = tracksystem->allresources;
	tex = loadImage(path);
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	track = tracksystem->tracks[0].get();
	nodedist = nodediststart;
	pos = track->getpos(nodedist);
}

void Wagon::update(int ms)
{
	float arclength1 = track->getarclength(1);
	nodedist += ms*0.001*train->speed*(alignedwithtrackdirection*2-1)/arclength1;
	if(nodedist>=1)
	{
		Node* currentnode = track->noderight;
		track = track->getrighttrack();
		float arclength2 = track->getarclength(1);
		if(track->nodeleft==currentnode)
			nodedist = (nodedist-1)*arclength1/arclength2;
		else{
			nodedist = 1-(nodedist-1)*arclength1/arclength2;
			alignedwithtrackdirection = 1-alignedwithtrackdirection;
		}
	}
	else if(nodedist<0)
	{
		Node* currentnode = track->nodeleft;
		track = track->getlefttrack();
		float arclength2 = track->getarclength(1);
		if(track->nodeleft==currentnode){
			nodedist = (-nodedist)*arclength1/arclength2;
			alignedwithtrackdirection = 1-alignedwithtrackdirection;
		}
		else{
			nodedist = 1-(-nodedist)*arclength1/arclength2;
		}
	}
	pos = track->getpos(nodedist);

	imageangle = track->getorientation(nodedist) + pi*!alignedwithtrackdirection + pi*!alignedforward;
}

void Wagon::render()
{
	int x = int(pos.x);
	int y = int(pos.y);
	SDL_Rect srcrect = {0, 0, w, h};
	SDL_Rect rect = {int(x - w / 2/scale), int(y - h / 2/scale), int(w/scale), int(h/scale)};
	SDL_RenderCopyEx(renderer, tex, &srcrect, &rect, -imageangle * 180 / pi, NULL, SDL_FLIP_NONE);
	if(loadedresource!=none){
		Resource* resource = allresources->get(loadedresource);
		resource->render(pos);
	}
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

Locomotive::Locomotive(Tracksystem& newtracksystem, float nodediststart) : Wagon(newtracksystem, nodediststart, "assets/loco0.png")
{
	h = h/imagenumber;
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
	SDL_Rect rect = {int(x - w / 2/scale), int(y - h / 2/scale), int(w/scale), int(h/scale)};
	SDL_RenderCopyEx(renderer, tex, &srcrect, &rect, -imageangle * 180 / pi, NULL, SDL_FLIP_NONE);
}

float Locomotive::getpower()
{
	if(abs(train->speed)<maxspeed[alignedforward==train->direction])
		return P[alignedforward==train->direction];
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

Openwagon::Openwagon(Tracksystem& newtracksystem, float nodediststart) : Wagon(newtracksystem, nodediststart, "assets/flakvagn1.png")
{}

int Openwagon::loadwagon(resourcetype type, int amount)
{
	int loadedamount = 0;
	if(type==hops||type==barley)
		loadedamount = Wagon::loadwagon(type, amount);
	return loadedamount;
}

Tankwagon::Tankwagon(Tracksystem& newtracksystem, float nodediststart) : Wagon(newtracksystem, nodediststart, "assets/train.png")
{}

int Tankwagon::loadwagon(resourcetype type, int amount)
{
	int loadedamount = 0;
	if(type==beer)
		loadedamount = Wagon::loadwagon(type, amount);
	return loadedamount;
}