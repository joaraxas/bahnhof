#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/track/trackinternal.h"



Track::Track(Tracksystem& newtracksystem, nodeid previous, nodeid next, trackid myid)
{
	previousnode = previous;
	nextnode = next;
	tracksystem = &newtracksystem;
	id = myid;
	Node* previousnodepointer = tracksystem->getnode(previousnode);
	Node* nextnodepointer = tracksystem->getnode(nextnode);

	previousdir = previousnodepointer->getdir();
	previouspos = tracksystem->getnode(previousnode)->getpos();
	nextdir = nextnodepointer->getdir();
	nextpos = tracksystem->getnode(nextnode)->getpos();
	float dx = cos(previousdir)*(nextpos.x - previouspos.x) - sin(previousdir)*(nextpos.y - previouspos.y);
	float dy = sin(previousdir)*(nextpos.x - previouspos.x) + cos(previousdir)*(nextpos.y - previouspos.y);
	radius = 0.5*(dy*dy+dx*dx)/dy;
	phi = sign(dy)*atan2(dx, sign(dy)*(radius-dy));
	if(abs(radius)>1e5){
		radius = INFINITY;
		phi = 0;
	}
}

Vec Track::getpos(float nodedist)
{
	return getpos(nodedist, 0);
}

Vec Track::getpos(float nodedist, float transverseoffset)
{
	Vec currentpos;
	Vec leftnodeoffsetpos = previouspos - Vec(sin(previousdir), cos(previousdir))*transverseoffset;
	if(isinf(radius)){
		Vec rightnodeoffsetpos = nextpos - Vec(sin(previousdir), cos(previousdir))*transverseoffset;
		currentpos = leftnodeoffsetpos + (rightnodeoffsetpos-leftnodeoffsetpos)*nodedist;
	}
	else{
		float ddx, ddy;
		ddx = (radius+transverseoffset)*sin(nodedist*phi);
		ddy = (radius+transverseoffset)*(1-cos(nodedist*phi));
		currentpos = Vec(leftnodeoffsetpos.x + cos(previousdir)*ddx+sin(previousdir)*ddy, leftnodeoffsetpos.y - sin(previousdir)*ddx+cos(previousdir)*ddy);
	}
	return currentpos;
}

State Track::getcloseststate(Vec pos)
{
	State closeststate(id, 0, true);
	float dx = cos(previousdir)*(pos.x - previouspos.x) - sin(previousdir)*(pos.y - previouspos.y);
	float dy = sin(previousdir)*(pos.x - previouspos.x) + cos(previousdir)*(pos.y - previouspos.y);
	if(isinf(radius)){
		if(isabovepreviousnode()){
			closeststate.nodedist = fmax(fmin(1, dx/getarclength(1)), 0);
			closeststate.alignedwithtrack = (dy>=0);
		}
		else{
			closeststate.nodedist = fmax(fmin(1, -dx/getarclength(1)), 0);
			closeststate.alignedwithtrack = (dy<=0);
		}
	}
	else{
		if(isabovepreviousnode()){
			float angle = atan2(dx, sign(radius)*(radius-dy));
			closeststate.nodedist = fmax(fmin(1, angle/abs(phi)), 0);
			closeststate.alignedwithtrack = sign(radius)*((pow(radius-dy, 2) + pow(dx, 2))) <= sign(radius)*pow(radius,2);
		}
		else{
			float angle = atan2(-dx, sign(radius)*(radius-dy));
			closeststate.nodedist = fmax(fmin(1, angle/abs(phi)), 0);
			closeststate.alignedwithtrack = sign(radius)*((pow(radius-dy, 2) + pow(dx, 2))) >= sign(radius)*pow(radius,2);
		}
	}
	return closeststate;
}

float Track::getarclength(float nodedist)
{
	float arclength;
	if(isinf(radius)){
		arclength = nodedist*norm(previouspos - nextpos);
	}
	else{
		arclength = nodedist*abs(radius*phi);
	}
	return arclength;
}

float Track::getorientation(float nodedist)
{
	return previousdir - nodedist*phi + pi*!isabovepreviousnode();
}

bool Track::isabovepreviousnode()
{
	if(isinf(radius)){
		if(abs(previouspos.y - nextpos.y)>1)
			return previouspos.y >= nextpos.y;
		else
			if(cos(tracksystem->getnode(previousnode)->getdir()) > 0)
				return (nextpos.x >= previouspos.x);
			else
				return (nextpos.x <= previouspos.x);
	}
	else
		return radius*phi >= 0;
}

bool Track::isbelownextnode()
{
	return cos(getorientation(1) - nextdir) > 0;
}

void Track::addsignal(State signalstate, signalid signal)
{
	signals[signalstate.nodedist] = signal;
}

signalid Track::nextsignal(State state, bool startfromtrackend, bool mustalign)
{
	if(state.track!=id) std::cout<<"Error in Track::nextsignal: called for incorrect track"<<std::endl;
	if(startfromtrackend) state.nodedist = 1-state.alignedwithtrack;
	signalid reachedsignal = 0;
	if(state.alignedwithtrack){
		for(auto const& [nodedist,signal]: signals){
			if(nodedist>state.nodedist){
				if(tracksystem->getsignalstate(signal).alignedwithtrack || !mustalign){
					reachedsignal = signal;
				}
			}
			if(reachedsignal) break;
		}
	}
	else{
		for(auto const& [nodedist,signal]: signals){
			if(nodedist<state.nodedist)
				if(!tracksystem->getsignalstate(signal).alignedwithtrack || !mustalign)
					reachedsignal = signal;
		}
	}
	return reachedsignal;
}

void Track::render(Rendering* r, int mode)
{
	float scale = r->getscale();
	//// banvall ////
	/*if(nicetracks){
		SDL_SetRenderDrawColor(renderer, 127,127,127,255);
		float sleeperwidth = 4000/200;
		int nSleepers = round(getarclength(1)/0.25);
		if(isinf(radius)) nSleepers = round(getarclength(1));
		for(int iSleeper = 0; iSleeper < nSleepers; iSleeper++){
			float nodedist = float(iSleeper+0.5)/float(nSleepers);
			Vec drawposl = getpos(nodedist, sleeperwidth/2);
			Vec drawposr = getpos(nodedist, -sleeperwidth/2);
			if(drawposl.x>0)
			if(drawposl.x<SCREEN_WIDTH)
			if(drawposl.y>0)
			if(drawposl.y<SCREEN_HEIGHT){
				SDL_RenderDrawLine(renderer, drawposl.x, drawposl.y, drawposr.x, drawposr.y);
			}
		}
	}*/
	//// syllar ////
	if(nicetracks){
		SDL_SetRenderDrawColor(renderer, 63,63,0,255);
		if(mode==1)
			SDL_SetRenderDrawColor(renderer, 255,255,255,127);
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
	//// rals ////
	if(nicetracks){
		SDL_SetRenderDrawColor(renderer, 0,0,0,255);
		if(mode==1)
			SDL_SetRenderDrawColor(renderer, 255,255,255,127);
	}
	else SDL_SetRenderDrawColor(renderer, 255*isabovepreviousnode(),0, 255*isbelownextnode(),255);
	int nSegments = 1;
	if(!isinf(radius))
		nSegments = fmax(1,round(abs(phi/2/pi*4*128)));
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
		if(isinf(radius))
			radiustext = std::to_string(radius);
		r->rendertext(radiustext, midpoint.x, midpoint.y, {255,255,255,255});
		r->rendertext("track #"+std::to_string(id), midpoint.x, midpoint.y+14, {255,255,255,255});
	}
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
}
