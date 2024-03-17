#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/rendering.h"
#include "bahnhof/track/track.h"
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

void Signal::render(Rendering* r)
{
	if(!nicetracks){
		if(reservedfor){
			//rendertext(reservedfor->route->name, pos.x, pos.y+14);
		}
		else
			r->rendertext("noone", pos.x, pos.y+14);
	}
	if(r->getscale()>0.3){
			int w = tracksystem->signalrect.w; int h = tracksystem->signalrect.h;
			SDL_Rect rect = {int(pos.x-w*0.5), int(pos.y-h*0.5), w, h};
			tracksystem->signalrect.x = tracksystem->signalrect.w*isgreen;
			r->rendertexture(tracksystem->signaltex, &rect, &tracksystem->signalrect, 0, true, true);
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