#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/track/trackinternal.h"
#include "bahnhof/common/gamestate.h"

namespace Tracks{
Node::Node(Tracksystem& t, Vec p, float dirstart, nodeid id) : tracksystem(&t), pos(p), id(id)
{
	dir = truncate(dirstart);
}

void Node::connecttrack(Track* track, bool fromabove){
	if(fromabove){
		if(trackup==nullptr)
			trackup = track;
		else if(!switchup)
			switchup = std::unique_ptr<Switch>(new Switch(this, trackup, true));
		if(switchup)
			switchup->addtrack(track);
	}
	else{
		if(trackdown==nullptr)
			trackdown = track;
		else if(!switchdown)
			switchdown = std::unique_ptr<Switch>(new Switch(this, trackdown, false));
		if(switchdown)
			switchdown->addtrack(track);
	}
}

void Node::disconnecttrack(Track* track, bool fromabove)
{
	if(fromabove){
		if(switchup){
			switchup->removetrack(track);
			if(switchup->tracks.size()<=1)
				switchup.reset(); //delete
		}
		else
			trackup = nullptr;
	}
	else{
		if(switchdown){
			switchdown->removetrack(track);
			if(switchdown->tracks.size()<=1)
				switchdown.reset(); //delete
		}
		else
			trackdown = nullptr;
	}
	if(trackup==nullptr && trackdown==nullptr)
		tracksystem->removenode(id);
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
		if(trackdown)
			r->rendertext("track down: "+std::to_string(trackdown->id), pos.x, pos.y+2*14);
		if(trackup)
			r->rendertext("track up: "+std::to_string(trackup->id), pos.x, pos.y+3*14);
		r->rendertext(std::to_string(dir), pos.x, pos.y+4*14);
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
}