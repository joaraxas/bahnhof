#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/gamestate.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/track/track.h"
#include "bahnhof/track/trackinternal.h"
#include "bahnhof/graphics/rendering.h"

Signal::Signal(Tracksystem& newtracksystem, State signalstate, signalid myid) : id(myid), state(signalstate)
{
	tracksystem = &newtracksystem;
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
			r->rendertext("noone", pos().x, pos().y+14);
	}
	sprite.imagetype = isgreen;
	sprite.render(r, pos());
}

void Signal::update()
{
	blocks = tracksystem->getblocksuptonextsignal(state);
	if(checkblocks(*tracksystem, blocks, nullptr))
		isgreen = true;
	else
		isgreen = false;
}

void Signal::set(int redgreenorflip)
{
	if(redgreenorflip==2)
		isgreen = !isgreen;
	else if(redgreenorflip==0 || redgreenorflip==1)
		isgreen = redgreenorflip;
	else
		std::cout<<"Error: attempt to set signal "<<id<<" to illegal state "<<redgreenorflip<<std::endl;
}

bool Signal::isred(Train* fortrain)
{
	if(tracksystem->distancefromto(fortrain->forwardstate(), state, 100, true)<100){
		if(claimblocks(*tracksystem, blocks, fortrain))
			isgreen=true;
		if(!isgreen)
			return true;
	}
	return false;
}

Vec Signal::pos()
{
	float orientation = getorientation(*tracksystem, state);
	float transverseoffset = -20;
	return getpos(*tracksystem, state) - Vec(sin(orientation), cos(orientation))*transverseoffset;
}

int Signal::getcolorforordergeneration()
{
	// for generating a route order to set the color to the current one. Remove this when proper UI supports color choice
	return isgreen;
}