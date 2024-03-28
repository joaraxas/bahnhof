#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/track/trackinternal.h"


Vec getpos(Tracksystem& tracks, State state, float transverseoffset)
{
	return tracks.gettrack(state.track)->getpos(state.nodedist, transverseoffset);
}

float getorientation(Tracksystem& tracks, State state)
{
	return tracks.gettrack(state.track)->getorientation(state.nodedist) + pi*!state.alignedwithtrack;
}

float getradius(Tracksystem& tracks, State state)
{
	return tracks.gettrack(state.track)->getradius(state);
}

void setswitch(Tracksystem& tracks, switchid _switch, int switchstate)
{
	Switch* switchptr = tracks.getswitch(_switch);
	switchptr->setswitch(switchstate);
}

Vec getswitchpos(Tracksystem& tracks, switchid _switch)
{
	Switch* switchptr = tracks.getswitch(_switch);
	return switchptr->pos();
}

void setsignal(Tracksystem& tracks, signalid signal, int redgreenorflip)
{
	Signal* signalpointer = tracks.getsignal(signal);
	signalpointer->set(redgreenorflip);
}

Vec getsignalpos(Tracksystem& tracks, signalid signal)
{
	return tracks.getsignal(signal)->pos();
}

bool checkblocks(Tracksystem& tracks, Trackblock blocks, Train* fortrain)
{
	// TODO: Move this to Signal?
	for(auto s : blocks.switchblocks){
		Node* sw = tracks.getnode(s);
		if(sw->reservedfor && sw->reservedfor!=fortrain)
			return false;
	}
	for(auto s : blocks.signalblocks){
		Signal* si = tracks.getsignal(s);
		if(si->reservedfor && si->reservedfor!=fortrain)
			return false;
	}
	return true;
}

bool claimblocks(Tracksystem& tracks, Trackblock blocks, Train* fortrain)
{
	// TODO: Move this to Signal?
	if(checkblocks(tracks, blocks, fortrain)){
		for(auto s : blocks.switchblocks){
			Node* sw = tracks.getnode(s);
			sw->reservedfor = fortrain;
		}
		for(auto s : blocks.signalblocks){
			Signal* si = tracks.getsignal(s);
			si->reservedfor = fortrain;
		}
		return true;
	}
	else return false;
}