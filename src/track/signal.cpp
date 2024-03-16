#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<string>
#include<map>
#include "bahnhof/utils.h"
#include "bahnhof/common/constants.h"
#include "bahnhof/rollingstock/rollingstock.h"


Signal::Signal(Tracksystem& newtracksystem, State signalstate)
{
	tracksystem = &newtracksystem;
	state = signalstate;
	float orientation = tracksystem->getorientation(state);
	float transverseoffset = -20;
	pos = tracksystem->getpos(state) - Vec(sin(orientation), cos(orientation))*transverseoffset;
    tracksystem->setblocksuptonextsignal(this);
}

void Signal::render()
{
	if(!nicetracks){
		if(reservedfor){
			//rendertext(reservedfor->route->name, pos.x, pos.y+14);
		}
		else
			rendertext("noone", pos.x, pos.y+14);
	}
	if(scale>0.3){
			int w = tracksystem->signalrect.w; int h = tracksystem->signalrect.h;
			SDL_Rect rect = {int(pos.x-w*0.5), int(pos.y-h*0.5), w, h};
			tracksystem->signalrect.x = tracksystem->signalrect.w*isgreen;
			rendertexture(tracksystem->signaltex, &rect, &tracksystem->signalrect, 0*tracksystem->getorientation(state)-0*pi/2, true, true);
	}
}

bool Signal::isred(Train* fortrain)
{
	if(tracksystem->distancefromto(fortrain->forwardstate(), state, 100, true)<100){
		if(tracksystem->claimblocks(switchblocks, signalblocks, fortrain))
			isgreen=true;
		if(!isgreen)
			return true;
	}
	return false;
}