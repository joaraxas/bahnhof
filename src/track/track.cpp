#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/track/trackinternal.h"


namespace Tracks
{
Track::Track(Tracksystem& newtracksystem, Node& previous, Node& next, trackid myid)
{
	tracksystem = &newtracksystem;
	previousnode = &previous;
	nextnode = &next;
	id = myid;

	// TODO: Could also define a localcoords for tangent or use angledown depending on track connection
	Vec d = localcoords(nextnode->getpos(), previousnode->getdir().angleup(), previousnode->getpos());
	float dx = d.x; float dy = -d.y;
	radius = 0.5*(dy*dy+dx*dx)/dy;
	phi = sign(dy)*atan2(dx, sign(dy)*(radius-dy));
	if(abs(radius)>1e5){
		radius = INFINITY;
		phi = 0;
	}
}

Track::~Track()
{
	for(auto [state, signal]: signals)
		tracksystem->removesignal(signal);
}

void Track::connecttonodes()
{
	previousnode->connecttrack(this, isabovepreviousnode());
	nextnode->connecttrack(this, !isbelownextnode());
}

void Track::disconnectfromnodes()
{
	bool aboveprevious = isabovepreviousnode();
	bool abovenext = !isbelownextnode();
	previousnode->disconnecttrack(this, aboveprevious);
	nextnode->disconnecttrack(this, abovenext);
}

Vec Track::getpos(float nodedist, float transverseoffset)
{
	Vec currentpos;
	Vec previousoffsetpos = globalcoords(Vec(0,transverseoffset), previousnode->getdir().angleup(), previousnode->getpos());
	if(std::isinf(radius)){
		Vec nextoffsetpos = globalcoords(Vec(0,transverseoffset), nextnode->getdir().angleup(), nextnode->getpos());
		currentpos = previousoffsetpos + (nextoffsetpos-previousoffsetpos)*nodedist;
	}
	else{
		Vec localpos;
		localpos.x = (radius+transverseoffset)*sin(nodedist*phi);
		localpos.y =-(radius+transverseoffset)*(1-cos(nodedist*phi));
		currentpos = globalcoords(localpos, previousnode->getdir().angleup(), previousoffsetpos);	
	}
	return currentpos;
}

State Track::getcloseststate(Vec pos)
{
	State closeststate(id, 0, true);
	Vec d = localcoords(pos, previousnode->getdir().angleup(), previousnode->getpos());
	float dx = d.x; float dy = d.y;
	if(std::isinf(radius)){
		if(isabovepreviousnode()){
			closeststate.nodedist = fmax(fmin(1, dx/getarclength(1)), 0);
			closeststate.alignedwithtrack = (dy<=0);
		}
		else{
			closeststate.nodedist = fmax(fmin(1, -dx/getarclength(1)), 0);
			closeststate.alignedwithtrack = (dy>=0);
		}
	}
	else{
		if(isabovepreviousnode()){
			float angle = atan2(dx, sign(radius)*(radius+dy));
			closeststate.nodedist = fmax(fmin(1, angle/abs(phi)), 0);
			closeststate.alignedwithtrack = sign(radius)*((pow(radius+dy, 2) + pow(dx, 2))) <= sign(radius)*pow(radius,2);
		}
		else{
			float angle = atan2(-dx, sign(radius)*(radius+dy));
			closeststate.nodedist = fmax(fmin(1, angle/abs(phi)), 0);
			closeststate.alignedwithtrack = sign(radius)*((pow(radius+dy, 2) + pow(dx, 2))) >= sign(radius)*pow(radius,2);
		}
	}
	return closeststate;
}

float Track::getarclength(float nodedist)
{
	float arclength;
	if(std::isinf(radius)){
		arclength = nodedist*norm(previousnode->getpos() - nextnode->getpos());
	}
	else{
		arclength = nodedist*abs(radius*phi);
	}
	return arclength;
}

float Track::getorientation(float nodedist)
{
	return previousnode->getdir().angleup() - nodedist*phi + pi*!isabovepreviousnode();
}

float Track::getradius(State state)
{
	return radius*(2*state.alignedwithtrack-1)*(2*isabovepreviousnode()-1);
}

Track* Track::nexttrack(){
	Track* nexttrack;
	if(isbelownextnode()){
		nexttrack = nextnode->trackup;
	}
	else
		nexttrack = nextnode->trackdown;
	return nexttrack;
}

Track* Track::previoustrack(){
	Track* previoustrack;
	if(isabovepreviousnode())
		previoustrack = previousnode->trackdown;
	else
		previoustrack = previousnode->trackup;
	return previoustrack;
}

bool Track::isabovepreviousnode()
{
	if(std::isinf(radius)){
		if(abs(previousnode->getpos().y - nextnode->getpos().y)>1)
			return previousnode->getpos().y >= nextnode->getpos().y;
		else
			if(cos(previousnode->getdir()) > 0)
				return (nextnode->getpos().x >= previousnode->getpos().x);
			else
				return (nextnode->getpos().x <= previousnode->getpos().x);
	}
	else
		return radius*phi >= 0;
}

bool Track::isbelownextnode()
{
	return cos(getorientation(1) - nextnode->getdir().angleup()) > 0; // TODO: Will this work if the difference is small and dir is close to horizontal?
}

void Track::split(Track& track1, Track& track2, State where)
{
	for(auto& [nodedist, signal]: signals){
		Signal* signalptr = tracksystem->getsignal(signal);
		signalptr->state = getnewstateaftersplit(track1, track2, where, signalptr->state);
		signalptr->addtotrack();
	}
	signals.clear();
}

State Track::getnewstateaftersplit(Track& track1, Track& track2, State wheresplit, State oldstate)
{
	if(oldstate.track == id){
		if(oldstate.nodedist<wheresplit.nodedist){
			oldstate.track = track1.id;
			oldstate.nodedist = oldstate.nodedist/wheresplit.nodedist;
		}
		else{
			oldstate.track = track2.id;
			oldstate.nodedist = (oldstate.nodedist-wheresplit.nodedist)/(1-wheresplit.nodedist);
		}
	}
	return oldstate;
}

void Track::connectsignal(State signalstate, signalid signal)
{
	signals[signalstate.nodedist] = signal;
}

void Track::disconnectsignal(signalid signal)
{
	for(auto it = signals.begin(); it != signals.end(); it++)
	{
		if((it->second) == signal)
		{
			signals.erase(it);
			break;
		}
	}
}

signalid Track::nextsignal(State state, bool startfromtrackend, bool mustalign)
{
	if(state.track!=id) std::cout<<"Error in Track::nextsignal: called for incorrect track"<<std::endl;
	if(startfromtrackend) state.nodedist = 1-state.alignedwithtrack;
	signalid reachedsignal = 0;
	if(state.alignedwithtrack){
		for(auto const& [nodedist,signal]: signals){
			if(nodedist>state.nodedist){
				if(tracksystem->getsignal(signal)->state.alignedwithtrack || !mustalign){
					reachedsignal = signal;
				}
			}
			if(reachedsignal) break;
		}
	}
	else{
		for(auto const& [nodedist,signal]: signals){
			if(nodedist<state.nodedist)
				if(!tracksystem->getsignal(signal)->state.alignedwithtrack || !mustalign)
					reachedsignal = signal;
		}
	}
	return reachedsignal;
}

void Track::renderballast(Rendering* r, TracksDisplayMode mode)
{
	float scale = r->getcamscale();
	//// ballast ////
	if(nicetracks && scale>0.3){
		SDL_Color ballastcolor;
		switch(mode){
			case TracksDisplayMode::normal:
				ballastcolor = {127,127,127,255};
				break;
			case TracksDisplayMode::planned:
				ballastcolor = {255,255,255,63};
				break;
			case TracksDisplayMode::impossible:
				ballastcolor = {127,0,0,127};
				break;
		}
		std::unique_ptr<Shape> shape = getcollisionmask();
		shape->renderfilled(r, ballastcolor);
	}
}

void Track::render(Rendering* r, TracksDisplayMode mode)
{
	float scale = r->getcamscale();
	//// sleepers ////
	if(nicetracks){
		switch(mode){
			case TracksDisplayMode::normal:
				SDL_SetRenderDrawColor(renderer, 63,63,0,255);
				break;
			case TracksDisplayMode::planned:
				SDL_SetRenderDrawColor(renderer, 255,255,255,127);
				break;
			case TracksDisplayMode::impossible:
				SDL_SetRenderDrawColor(renderer, 220,0,0,255);
				break;
		}
		float sleeperwidth = 2600/150;
		if(scale<0.2) sleeperwidth = 2600/150*0.25/scale;
		int nSleepers = round(getarclength(1)/3*fmin(1,scale));
		if(scale<0.3) nSleepers = round(getarclength(1)/20*fmin(1,scale));
		for(int iSleeper = 0; iSleeper < nSleepers; iSleeper++){
			float nodedist = float(iSleeper+0.5)/float(nSleepers);
			Vec drawposl = getpos(nodedist, sleeperwidth/2);
			Vec drawposr = getpos(nodedist, -sleeperwidth/2);{
				r->renderline(drawposl, drawposr);
			}
		}
	}
	//// rails ////
	if(nicetracks){
		switch(mode){
			case TracksDisplayMode::normal:
				SDL_SetRenderDrawColor(renderer, 0,0,0,255);
				break;
			case TracksDisplayMode::planned:
				SDL_SetRenderDrawColor(renderer, 255,255,255,127);
				break;
			case TracksDisplayMode::impossible:
				SDL_SetRenderDrawColor(renderer, 255,0,0,255);
				break;
		}
	}
	else SDL_SetRenderDrawColor(renderer, 255*isabovepreviousnode(),0, 255*isbelownextnode(),255);
	int nSegments = 1;
	if(!std::isinf(radius))
		nSegments = fmax(1,round(abs(phi/pi*32*radius/100)));
	float gauge = 0;
	if(nicetracks && scale>0.3) gauge = normalgauge*1000/150;
	for(int iSegment = 0; iSegment < nSegments; iSegment++){
		float nodedist = float(iSegment)/float(nSegments);
		Vec drawpos1l = getpos(nodedist, gauge/2);
		Vec drawpos2l = getpos(nodedist+1./nSegments, gauge/2);
		r->renderline(drawpos1l, drawpos2l);
	}
	if(gauge!=0)
	for(int iSegment = 0; iSegment < nSegments; iSegment++){
		float nodedist = float(iSegment)/float(nSegments);
		Vec drawpos1r = getpos(nodedist, -gauge/2);
		Vec drawpos2r = getpos(nodedist+1./nSegments, -gauge/2);
		r->renderline(drawpos1r, drawpos2r);
	}
	if(!nicetracks){
		Vec midpoint = getpos(0.5);
		std::string radiustext = std::to_string(int(round(radius*150*0.001))) + " m";
		if(std::isinf(radius))
			radiustext = std::to_string(radius);
		r->rendertext(radiustext, midpoint.x, midpoint.y, {255,255,255,255});
		r->rendertext("track #"+std::to_string(id), midpoint.x, midpoint.y+14/scale, {255,255,255,255});
		r->rendertext("phi "+std::to_string(phi), midpoint.x, midpoint.y+28/scale, {255,255,255,255});
	}
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
}

std::unique_ptr<Shape> Track::getcollisionmask()
{
	float ballastwidth = 4000/150;
	if(std::isinf(radius))
		return std::make_unique<RotatedRectangle>(
			getpos(0.5, 0), 
			getarclength(1), 
			ballastwidth,
			getorientation(0)
		);
	return std::make_unique<AnnularSector>(
			getpos(0, 0), 
			getorientation(0),
			getpos(1, 0),
			ballastwidth
		);
}

} //namespace Tracks