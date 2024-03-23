#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/common/gamestate.h"


Signal::Signal(Tracksystem& newtracksystem, State signalstate)
{
	tracksystem = &newtracksystem;
	state = signalstate;
	float orientation = tracksystem->getorientation(state);
	float transverseoffset = -20;
	pos = tracksystem->getpos(state) - Vec(sin(orientation), cos(orientation))*transverseoffset;
    tracksystem->setblocksuptonextsignal(this);
	sprite.setspritesheet(tracksystem->game->getsprites(), sprites::signal);
	sprite.zoomed = false;
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
	sprite.imagetype = isgreen;
	sprite.render(r, pos);
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