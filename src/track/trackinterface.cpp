#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/track/trackinternal.h"

namespace Tracks{
Vec getpos(Tracksystem& tracksystem, State state, float transverseoffset)
{
	return tracksystem.gettrack(state.track)->getpos(state.nodedist, transverseoffset);
}

float getorientation(Tracksystem& tracksystem, State state)
{
	return tracksystem.gettrack(state.track)->getorientation(state.nodedist) + pi*!state.alignedwithtrack;
}

float getradius(Tracksystem& tracksystem, State state)
{
	return tracksystem.gettrack(state.track)->getradius(state);
}

void setswitch(Tracksystem& tracksystem, switchid _switch, int switchstate)
{
	Switch* switchptr = tracksystem.getswitch(_switch);
	switchptr->setswitch(switchstate);
}

Vec getswitchpos(Tracksystem& tracksystem, switchid _switch)
{
	Switch* switchptr = tracksystem.getswitch(_switch);
	return switchptr->pos();
}

void setsignal(Tracksystem& tracksystem, signalid signal, int redgreenorflip)
{
	Signal* signalpointer = tracksystem.getsignal(signal);
	signalpointer->set(redgreenorflip);
}

Vec getsignalpos(Tracksystem& tracksystem, signalid signal)
{
	return tracksystem.getsignal(signal)->pos();
}

void render(Tracksystem& tracksystem, Rendering* r)
{
	render(Tracksection(tracksystem.alltracks(), tracksystem.allnodes()), r);
}

void render(Tracksection section, Rendering* r, int mode)
{
	for(auto const track : section.tracks)
		track->render(r, mode);
	for(auto const node : section.nodes)
		node->render(r);
}

void renderabovetrains(Tracksystem& tracksystem, Rendering* r)
{
	for(auto const& _switch : tracksystem.allswitches())
		_switch->render(r);
	for(auto const& signal : tracksystem.allsignals())
		signal->render(r);
}

State tryincrementingtrack(Tracksystem& tracks, State oldstate)
{
	State state = oldstate;
	if(state.nodedist>=1){
		Track* currenttrackpointer = tracks.gettrack(state.track);
		Node* currentnode = currenttrackpointer->nextnode;
		float arclength1 = currenttrackpointer->getarclength(1);
		Track* nexttrackpointer = currenttrackpointer->nexttrack();
		if(nexttrackpointer){
			state.track = nexttrackpointer->id;
			float arclength2 = nexttrackpointer->getarclength(1);
			if(nexttrackpointer->previousnode==currentnode){
				state.nodedist = (state.nodedist-1)*arclength1/arclength2;
			}
			else{
				state.nodedist = 1-(state.nodedist-1)*arclength1/arclength2;
				state.alignedwithtrack = !state.alignedwithtrack;
			}
		}
		else{
			state.track = oldstate.track;
			state.nodedist = 1;
		}
	}
	else if(state.nodedist<0){
		Track* currenttrackpointer = tracks.gettrack(state.track);
		Node* currentnode = currenttrackpointer->previousnode;
		float arclength1 = currenttrackpointer->getarclength(1);
		Track* previoustrackpointer = currenttrackpointer->previoustrack();
		if(previoustrackpointer){
			state.track = previoustrackpointer->id;
			float arclength2 = previoustrackpointer->getarclength(1);
			if(previoustrackpointer->nextnode==currentnode){
				state.nodedist = 1-(-state.nodedist)*arclength1/arclength2;
			}
			else{
				state.nodedist = (-state.nodedist)*arclength1/arclength2;
				state.alignedwithtrack = !state.alignedwithtrack;
			}
		}
		else{
			state.track = oldstate.track;
			state.nodedist = 0;
		}
	}
	return state;
}

State travel(Tracksystem& tracks, State state, float pixels)
{
	bool finishedtrip = false;

	float arclength1 = tracks.gettrack(state.track)->getarclength(1);
	state.nodedist += pixels*((state.alignedwithtrack)*2-1)/arclength1;

	while(!finishedtrip){
		trackid currenttrack = state.track;
		state = tryincrementingtrack(tracks, state);
		if(state.track==currenttrack) finishedtrip = true;
	}

	return state;
}

float distancefromto(Tracksystem& tracks, State state1, State state2, float maxdist, bool mustalign)
{
	State state = state1;
	float arclength = tracks.gettrack(state.track)->getarclength(1);
	bool movingalongtrack = (maxdist>0)==state1.alignedwithtrack;
	if(state1.track==state2.track && (!mustalign || state1.alignedwithtrack==state2.alignedwithtrack)){
		if(movingalongtrack){
			if(state2.nodedist>=state1.nodedist)
				return arclength*(state2.nodedist-state1.nodedist);
		}
		else
			if(state2.nodedist<=state1.nodedist)
				return arclength*(state1.nodedist-state2.nodedist);
	}

	float distance = 0;
	if(movingalongtrack)
		distance = arclength*(1-state.nodedist);
	else
		distance = arclength*state.nodedist;
	state.nodedist += maxdist*((state.alignedwithtrack)*2-1)/arclength;
	if(state.nodedist>=0 && state.nodedist<=1)
		distance = maxdist;
	else{
		bool finishedtrip = false;
		while(!finishedtrip){
			trackid currenttrack = state.track;
			state = tryincrementingtrack(tracks, state);
			if(state.track==state2.track && (!mustalign || state.alignedwithtrack==state2.alignedwithtrack)){
				finishedtrip = true;
				movingalongtrack = (maxdist>0)==state.alignedwithtrack;
				if(movingalongtrack)
					distance += tracks.gettrack(state.track)->getarclength(1)*state2.nodedist;
				else
					distance += tracks.gettrack(state.track)->getarclength(1)*(1-state2.nodedist);
			}
			else if(state.track==currenttrack){
				distance = maxdist;
			}
			else
				distance += tracks.gettrack(state.track)->getarclength(1);
			if(abs(distance)>=abs(maxdist)){
				finishedtrip = true;
				distance = maxdist;
			}
		}
	}

	return distance;
}

bool isendofline(Tracksystem& tracksystem, State state)
{
	if(state.nodedist<=0)
		if(!state.alignedwithtrack)
			if(tracksystem.gettrack(state.track)->previoustrack()==nullptr)
				return true;
	if(state.nodedist>=1)
		if(state.alignedwithtrack)
			if(tracksystem.gettrack(state.track)->nexttrack()==nullptr)
				return true;
	return false;
}

}