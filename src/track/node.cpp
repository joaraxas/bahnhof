#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/common/gamestate.h"

Node::Node(Tracksystem& newtracksystem, Vec posstart, float dirstart, nodeid myid)
{
	tracksystem = &newtracksystem;
	id = myid;
	pos = posstart;
	dir = truncate(dirstart);
}

void Node::connecttrack(trackid track, bool fromabove){
	if(fromabove){
		std::cout<<"node"<<id<<"from above"<<std::endl;
		if(trackup==0)
			trackup = track;
		else if(!switchup)
			switchup = std::unique_ptr<Switch>(new Switch(this, trackup, true));
		if(switchup)
			switchup->addtrack(track);
	}
	else{
		std::cout<<"node"<<id<<"from below"<<std::endl;
		if(trackdown==0)
			trackdown = track;
		else if(!switchdown)
			switchdown = std::unique_ptr<Switch>(new Switch(this, trackdown, false));
		if(switchdown)
			switchdown->addtrack(track);
	}
}

void Node::render(Rendering* r)
{
	float scale = r->getscale();
	if(!nicetracks){
		r->renderline(pos+Vec(-5,-5)/scale, pos+Vec(5,5)/scale);
		r->renderline(pos+Vec(-5,5)/scale, pos+Vec(5,-5)/scale);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		r->renderline(pos, pos+Vec(12*cos(dir),-12*sin(dir))/scale);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		r->rendertext(std::to_string(id), pos.x, pos.y);
		if(reservedfor){
			//rendertext(reservedfor->route->name, pos.x, pos.y+14);
		}
		else
			r->rendertext("noone", pos.x, pos.y+14);
	}
}

Vec getswitchpos(Vec pos, float dir, bool updown)
{
	float transverseoffset = -(2*updown-1)*28;///scale;
	return pos - Vec(sin(dir), cos(dir))*transverseoffset;
}
