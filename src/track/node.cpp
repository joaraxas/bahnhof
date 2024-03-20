#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"

Node::Node(Tracksystem& newtracksystem, Vec posstart, float dirstart, nodeid myid)
{
	tracksystem = &newtracksystem;
	id = myid;
	pos = posstart;
	dir = truncate(dirstart);
	stateup = 0;
	statedown = 0;
}

void Node::connecttrack(trackid track, bool fromabove){
	if(fromabove){
		int insertionindex = 0;
		if(tracksup.size()>=1){
			insertionindex = tracksup.size();
			float newtrackcurvature = 1./tracksystem->getradiusoriginatingfromnode(id, track);
			for(int iTrack=0; iTrack<tracksup.size(); iTrack++){
				if(newtrackcurvature < 1./tracksystem->getradiusoriginatingfromnode(id, tracksup[iTrack]))
					insertionindex = iTrack;
				if(insertionindex==iTrack) break;
			}
		}
		tracksup.insert(tracksup.begin()+insertionindex, track);
	}
	else{
		int insertionindex = 0;
		if(tracksdown.size()>=1){
			insertionindex = tracksdown.size();
			float newtrackcurvature = 1./tracksystem->getradiusoriginatingfromnode(id, track);
			for(int iTrack=0; iTrack<tracksdown.size(); iTrack++){
				if(newtrackcurvature < 1./tracksystem->getradiusoriginatingfromnode(id, tracksdown[iTrack]))
					insertionindex = iTrack;
				if(insertionindex==iTrack) break;
			}
		}
		tracksdown.insert(tracksdown.begin()+insertionindex, track);
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
	if(scale>0.3){
		if(size(tracksup)>1){
			Vec switchpos = getswitchpos(true);
			int w = tracksystem->switchrect.w; int h = tracksystem->switchrect.h;
			SDL_Rect rect = {int(switchpos.x-w*0.5), int(switchpos.y-h*0.5), w, h};
			tracksystem->switchrect.y = tracksystem->switchrect.h*stateup;
			r->rendertexture(tracksystem->switchtex, &rect, &tracksystem->switchrect, dir-pi/2, true, true);
			if(!nicetracks)
				r->rendertext(std::to_string(stateup), switchpos.x, switchpos.y+7, {0,0,0,0});
		}
		if(size(tracksdown)>1){
			Vec switchpos = getswitchpos(false);
			int w = tracksystem->switchrect.w; int h = tracksystem->switchrect.h;
			SDL_Rect rect = {int(switchpos.x-w*0.5), int(switchpos.y-h*0.5), w, h};
			tracksystem->switchrect.y = tracksystem->switchrect.h*statedown;
			r->rendertexture(tracksystem->switchtex, &rect, &tracksystem->switchrect, pi+dir-pi/2, true, true);
			if(!nicetracks)
				r->rendertext(std::to_string(statedown), switchpos.x, switchpos.y+7, {0,0,0,0});
		}
	}
}

Vec Node::getswitchpos(bool updown)
{
	float transverseoffset = -(2*updown-1)*28;///scale;
	return pos - Vec(sin(dir), cos(dir))*transverseoffset;
}

trackid Node::gettrackdown()
{
	trackid trackdown = 0;
	if(tracksdown.size() > 0)
		trackdown = tracksdown[statedown];
	return trackdown;
}

trackid Node::gettrackup()
{
	trackid trackup = 0;
	if(tracksup.size() > 0)
		trackup = tracksup[stateup];
	return trackup;
}

void Node::incrementswitch(bool updown)
{
	if(updown){
		stateup++;
		if(stateup>=tracksup.size())
			stateup = 0;
	}
	else{
		statedown++;
		if(statedown>=tracksdown.size())
			statedown = 0;
	}
}
