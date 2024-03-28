#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/track/trackinternal.h"
#include "bahnhof/common/gamestate.h"

Node::Node(Tracksystem& t, Vec p, float dirstart, nodeid id) : tracksystem(&t), pos(p), id(id)
{
	dir = truncate(dirstart);
}

void Node::connecttrack(Track* track, bool fromabove){
	if(fromabove){
		if(trackup==0)
			trackup = track->id;
		else if(!switchup)
			switchup = std::unique_ptr<Switch>(new Switch(this, tracksystem->gettrack(trackup), true));
		if(switchup)
			switchup->addtrack(track);
	}
	else{
		if(trackdown==0)
			trackdown = track->id;
		else if(!switchdown)
			switchdown = std::unique_ptr<Switch>(new Switch(this, tracksystem->gettrack(trackdown), false));
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
		r->rendertext("track down: "+std::to_string(trackdown), pos.x, pos.y+2*14);
		r->rendertext("track up: "+std::to_string(trackup), pos.x, pos.y+3*14);
	}
}

Vec Node::getpos()
{
	return pos;
}

float Node::getdir()
{
	return dir;
}

bool Node::hasswitch()
{
	return switchup || switchdown;
}

Vec getswitchpos(Vec pos, float dir, bool updown)
{
	float transverseoffset = -(2*updown-1)*28;///scale;
	return pos - Vec(sin(dir), cos(dir))*transverseoffset;
}
