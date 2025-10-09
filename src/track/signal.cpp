#include<map>
#include "bahnhof/common/gamestate.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/track/track.h"
#include "bahnhof/track/trackinternal.h"
#include "bahnhof/graphics/rendering.h"

namespace Tracks{
Signal::Signal(Tracksystem& newtracksystem, State signalstate, signalid myid) : id(myid), state(signalstate)
{
	tracksystem = &newtracksystem;
	sprite.setspritesheet(tracksystem->game->getsprites(), sprites::signal);
	sprite.zoomed = false;
	pos = getsignalposfromstate(newtracksystem, state);
}

void Signal::disconnectfromtrack()
{
	tracksystem->gettrack(state.track)->disconnectsignal(id);
}

void Signal::addtotrack()
{
	tracksystem->gettrack(state.track)->connectsignal(state, id);
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

void Signal::update()
{
	blocks = Signaling::getblocksuptonextsignal(*tracksystem, state);
	if(Signaling::checkblocks(*tracksystem, blocks, nullptr))
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
	if(distancefromto(*tracksystem, fortrain->forwardstate(), state, 100, true)<100){
		if(Signaling::claimblocks(*tracksystem, blocks, fortrain))
			isgreen=true;
		if(!isgreen)
			return true;
	}
	return false;
}

Vec getsignalposfromstate(const Tracksystem& tracksystem, State state)
{
	Angle orientation = getorientation(tracksystem, state);
	float transverseoffset = -20;
	return getpos(tracksystem, state) - Vec(sin(orientation), cos(orientation))*transverseoffset;
}

int Signal::getcolorforordergeneration()
{
	// for generating a route order to set the color to the current one. Remove this when proper UI supports color choice
	return isgreen;
}

namespace Signaling{
void update(Tracksystem& tracks, int ms)
{
	for(auto const& signal : tracks.allsignals()){
		signal->update();
	}
}

bool isred(Tracksystem& tracksystem, Train* fortrain)
{
	bool red = false;
	for(auto const& signal : tracksystem.allsignals()){
		if(signal->isred(fortrain))
			red = true;
		if(red)
			break;
	}
	return red;
}

Trackblock getblocksuptonextsignal(Tracksystem& tracksystem, State state)
{
	bool reachedend = false;
	Trackblock blocks;
	State goalstate = state;
	signalid reachedsignal = tracksystem.gettrack(goalstate.track)->nextsignal(goalstate);
	goalstate.nodedist = (2*goalstate.alignedwithtrack-1)*INFINITY;
	nodeid passednode = 0;
	while(!reachedsignal && !reachedend){
		if(goalstate.alignedwithtrack)
			passednode = tracksystem.gettrack(goalstate.track)->nextnode->id;
		else
			passednode = tracksystem.gettrack(goalstate.track)->previousnode->id;
		if(tracksystem.getnode(passednode)->hasswitch()){
			if(std::find(blocks.switchblocks.begin(), blocks.switchblocks.end(), passednode) != blocks.switchblocks.end()){
				reachedend = true;
			}
			else{
				blocks.switchblocks.push_back(passednode);
			}
		}
		State newgoalstate = tryincrementingtrack(tracksystem, goalstate);
		if(newgoalstate.track==goalstate.track)
			reachedend = true;
		else{
			reachedsignal = tracksystem.gettrack(newgoalstate.track)->nextsignal(newgoalstate, true);
		}
		goalstate = newgoalstate;
	}
	if(reachedsignal)
		blocks.signalblocks.push_back(reachedsignal);
	return blocks;
}

void runoverblocks(Tracksystem& tracksystem, State state, float pixels, Train* fortrain)
{
	signalid nextsignal = tracksystem.gettrack(state.track)->nextsignal(state, false, false);
	if(nextsignal){
		Signal* signalpointer = tracksystem.getsignal(nextsignal);
		if(distancefromto(tracksystem, state, signalpointer->state, pixels)<pixels)
			signalpointer->reservedfor = fortrain;
	}
	State newstate = travel(tracksystem, state, pixels);
	if(newstate.track==state.track)
		return;
	for(auto const& node : tracksystem.allnodes()){
		if(node->hasswitch()){
			if(tracksystem.gettrack(state.track)->previousnode==node || tracksystem.gettrack(state.track)->nextnode==node)
			if(tracksystem.gettrack(newstate.track)->previousnode==node || tracksystem.gettrack(newstate.track)->nextnode==node)
				node->reservedfor=fortrain;
		}
	}
}

bool checkblocks(Tracksystem& tracksystem, Trackblock blocks, Train* fortrain)
{
	// TODO: Move this to Signal?
	for(auto s : blocks.switchblocks){
		Node* sw = tracksystem.getnode(s);
		if(sw->reservedfor && sw->reservedfor!=fortrain)
			return false;
	}
	for(auto s : blocks.signalblocks){
		Signal* si = tracksystem.getsignal(s);
		if(si->reservedfor && si->reservedfor!=fortrain)
			return false;
	}
	return true;
}

bool claimblocks(Tracksystem& tracksystem, Trackblock blocks, Train* fortrain)
{
	// TODO: Move this to Signal?
	if(checkblocks(tracksystem, blocks, fortrain)){
		for(auto s : blocks.switchblocks){
			Node* sw = tracksystem.getnode(s);
			sw->reservedfor = fortrain;
		}
		for(auto s : blocks.signalblocks){
			Signal* si = tracksystem.getsignal(s);
			si->reservedfor = fortrain;
		}
		return true;
	}
	else return false;
}
}

}
