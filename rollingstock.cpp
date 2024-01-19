#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<string>
#include "utils.h"

std::vector<std::unique_ptr<Wagon>> wagons;

Wagon::Wagon(Tracksystem* newtracksystem, float nodediststart)
{
	tracksystem = newtracksystem;
	tex = loadImage("assets/train.png");
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	h = h/2;
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
		if(cos(-(sign(track->radius*track->phi)-1)/2*pi+track->nodeleft->dir-track->phi-track->noderight->dir)>0){ //TODO: radius*phi is nan when straight
			track = currentnode->tracksright[currentnode->stateright];
		}
		else{
			track = currentnode->tracksleft[currentnode->stateleft];
		}
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
		//if(track->radius*track->phi>=0 || track->radius==INFINITY){
		if(track->radius*track->phi>=0){
			track = currentnode->tracksleft[currentnode->stateleft];
		}
		else{
			track = currentnode->tracksright[currentnode->stateright];
		}
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
	imageangle = track->nodeleft->dir - nodedist*track->phi + pi/2+pi/2*sign(track->phi*track->radius) + pi*alignedwithtrackdirection + pi*(1-alignedforward);
}

void Wagon::render()
{
	int x = int(pos.x);
	int y = int(pos.y);
	SDL_Rect srcrect = {0, 0, w, h};
	SDL_Rect rect = {int(x - w / 2), int(y - h / 2), int(w), int(h)};
	SDL_RenderCopyEx(renderer, tex, &srcrect, &rect, -imageangle * 180 / pi, NULL, SDL_FLIP_NONE);
	if(loadedresource!=nullptr)
		loadedresource->render(pos);
}

int Wagon::loadwagon(Resource &resource, int amount)
{
	int loadedamount = 0;
	if(loadedresource == &resource || loadedresource == nullptr){
		loadedamount = fmin(amount, maxamount - loadamount);
		loadamount += loadedamount;
		if(loadedamount>0)
			loadedresource = &resource;
		}
	return loadedamount;
}

void Wagon::unloadwagon()
{
	loadedresource = nullptr;
	loadamount = 0;
}