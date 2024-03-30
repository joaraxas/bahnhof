#include<iostream>
#include<string>
#include<map>
#include "bahnhof/routing/routing.h"
#include "bahnhof/track/trackinternal.h"
#include "bahnhof/track/track.h"
#include "bahnhof/common/input.h"
#include "bahnhof/common/camera.h"

namespace Tracks
{
namespace Input
{
float buildat(Tracksystem& tracksystem, Vec pos)
{
	float cost=0;
	if(tracksystem.placingsignal){
		trackid clickedtrack = 0;
		State signalstate = getcloseststate(tracksystem, pos);
		tracksystem.addsignal(signalstate);
		tracksystem.placingsignal = false;
		cost = 1;
	}
	else{
		//trackid lasttrackindex = trackcounter;
		nodeid clickednode = 0;
		whatdidiclick(tracksystem, pos, nullptr, &clickednode, nullptr, nullptr);
		if(clickednode){
			Construction::connecttwonodes(tracksystem, tracksystem.selectednode, clickednode);
			tracksystem.selectednode = clickednode;
		}
		else
			tracksystem.selectednode = Construction::extendtracktopos(tracksystem, tracksystem.selectednode, pos);
		//for(trackid id = lasttrackindex+1; id<=trackcounter; id++)
		//	cost += 0.003*tracksystem.gettrack(id)->getarclength(1);
	}
	return cost;
}

void selectat(Tracksystem& tracksystem, Vec pos)
{
	tracksystem.selectednode = 0;
	whatdidiclick(tracksystem, pos, nullptr, &tracksystem.selectednode, nullptr, nullptr);
}

bool switchat(Tracksystem& tracks, Vec pos)
{
	signalid clickedsignal=0; switchid clickedswitch=0;
	State clickedstate = whatdidiclick(tracks, pos, nullptr, nullptr, &clickedsignal, &clickedswitch);
	if(clickedsignal)
		setsignal(tracks, clickedsignal, 2);
	if(clickedswitch)
		setswitch(tracks, clickedswitch, -1);
	return(clickedsignal||clickedswitch);
}

Order* generateorderat(Tracksystem& tracks, Vec pos)
{
	Order* neworder = nullptr;
	trackid clickedtrack=0; signalid clickedsignal=0; nodeid clickedswitch=0;
	State clickedstate = whatdidiclick(tracks, pos, &clickedtrack, nullptr, &clickedsignal, &clickedswitch);
	if(clickedtrack)
		neworder = new Gotostate(clickedstate);
	if(clickedsignal)
		neworder = new Setsignal(clickedsignal, tracks.getsignal(clickedsignal)->getcolorforordergeneration());
	if(clickedswitch)
		neworder = new Setswitch(clickedswitch, tracks.getswitch(clickedswitch)->getstateforordergeneration());
	return neworder;
}

State whatdidiclick(Tracksystem& tracksystem, Vec mousepos, trackid* track, nodeid* node, signalid* signal, switchid* _switch)
{
	float trackdist = INFINITY, nodedist = INFINITY, signaldist = INFINITY, switchdist = INFINITY;
	State closeststate; nodeid closestnode = 0; signalid closestsignal = 0; nodeid closestswitch = 0;
	
	if(track){
		*track = 0;
		closeststate = getcloseststate(tracksystem, mousepos);
		if(closeststate.track)
			trackdist = distancebetween(getpos(tracksystem, closeststate), mousepos);
	}
	if(node){
		*node = 0;
		closestnode = getclosestnode(tracksystem, mousepos);
		if(closestnode)
			nodedist = distancebetween(tracksystem.getnode(closestnode)->getpos(), mousepos);
	}
	if(signal){
		*signal = 0;
		closestsignal = getclosestsignal(tracksystem, mousepos);
		if(closestsignal)
			signaldist = distancebetween(tracksystem.getsignal(closestsignal)->pos(), mousepos);
	}
	if(_switch){
		*_switch = 0;
		closestswitch = getclosestswitch(tracksystem, mousepos);
		if(closestswitch)
			switchdist = distancebetween(tracksystem.getswitch(closestswitch)->pos(), mousepos);
	}
	State returnstate;
	float mindist = std::min({trackdist, nodedist, signaldist, switchdist});
	if(mindist<20/tracksystem.game->getcamera().getscale()){
		if(trackdist==mindist){
			*track = closeststate.track;
			returnstate = closeststate;
		}
		else if(nodedist==mindist)
			*node = closestnode;
		else if(signaldist==mindist){
			*signal = closestsignal;
			returnstate = tracksystem.getsignal(closestsignal)->state;
		}
		else if(switchdist==mindist){
			*_switch = closestswitch;
		}
	}
	return returnstate;
}

void deleteclick(Tracksystem& tracksystem, int x, int y)
{
	tracksystem.selectednode = 0;
	Vec mousepos(x,y);
}

State getcloseststate(Tracksystem& tracksystem, Vec pos)
{
	float mindist = INFINITY;
	State closeststate;
	for(auto track: tracksystem.alltracks()){
		State state = track->getcloseststate(pos);
		float dist = distancebetween(track->getpos(state.nodedist), pos);
		if(dist < mindist){
			closeststate = state;
			mindist = dist;
		}
	}
	return closeststate;
}

nodeid getclosestnode(Tracksystem& tracksystem, Vec pos)
{
	float mindistsquared = INFINITY;
	nodeid closestnode = 0;
	for(auto const node: tracksystem.allnodes()){
		float distsquared = pow(node->getpos().x-pos.x, 2) + pow(node->getpos().y-pos.y, 2);
		if(distsquared < mindistsquared){
			closestnode = node->id;
			mindistsquared = distsquared;
		}
	}
	return closestnode;
}

signalid getclosestsignal(Tracksystem& tracksystem, Vec pos)
{
	float mindistsquared = INFINITY;
	signalid closestsignal = 0;
	for(auto signal: tracksystem.allsignals()){
		float distsquared = pow(signal->pos().x-pos.x, 2) + pow(signal->pos().y-pos.y, 2);
		if(distsquared < mindistsquared){
			closestsignal = signal->id;
			mindistsquared = distsquared;
		}
	}
	return closestsignal;
}

nodeid getclosestswitch(Tracksystem& tracks, Vec pos)
{
	float mindist = INFINITY;
	switchid closestswitch = 0;
	for(auto _switch: tracks.allswitches()){
		float dist = distancebetween(_switch->pos(), pos);
		if(dist < mindist){
			closestswitch = _switch->id;
			mindist = dist;
		}
	}
	return closestswitch;
}


}


}