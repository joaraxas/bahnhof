#include<iostream>
#include<string>
#include<map>
#include "bahnhof/routing/routing.h"
#include "bahnhof/track/trackinternal.h"
#include "bahnhof/track/track.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/input.h"
#include "bahnhof/common/camera.h"
#include "bahnhof/rollingstock/rollingstock.h"

namespace Tracks
{
namespace Input
{
signalid buildsignalat(Tracksystem& tracksystem, Vec pos)
{
	trackid clickedtrack = 0;
	State signalstate = getcloseststate(tracksystem, pos);
	return tracksystem.addsignal(signalstate);
}

Tracksection planconstructionto(Tracksystem& tracksystem, Node* fromnode, Vec pos)
{
	trackid clickedtrack = 0;
	State clickedstate = whatdidiclick(tracksystem, pos, &clickedtrack, nullptr, nullptr, nullptr);
	if(clickedtrack){
		nodeid clickednode = 0;
		whatdidiclick(tracksystem, pos, nullptr, &clickednode, nullptr, nullptr);
		Node* tonode;
		if(!clickednode){
			tonode = new Node(tracksystem, getpos(tracksystem, clickedstate), getorientation(tracksystem, clickedstate), -1);
		}
		else
			tonode = tracksystem.getnode(clickednode);
		Tracksection section = Construction::connecttwonodes(tracksystem, fromnode, tonode);
		if(!clickednode){
			section.nodes.push_back(tonode);
			section.tracksplits[tonode] = clickedstate;
		}
		return section;
	}
	return Construction::extendtracktopos(tracksystem, fromnode, pos);
}

Tracksection planconstructionto(Tracksystem& tracksystem, Vec frompos, Vec pos)
{
	trackid clickedtrack = 0;
	State clickedstate = whatdidiclick(tracksystem, pos, &clickedtrack, nullptr, nullptr, nullptr);
	if(clickedtrack){
		nodeid clickednode = 0;
		whatdidiclick(tracksystem, pos, nullptr, &clickednode, nullptr, nullptr);
		Node* tonode;
		if(!clickednode){
			tonode = new Node(tracksystem, getpos(tracksystem, clickedstate), getorientation(tracksystem, clickedstate), -1);
		}
		else
			tonode = tracksystem.getnode(clickednode);
		Tracksection section = Construction::extendtracktopos(tracksystem, tonode, frompos);
		if(!clickednode){
			section.nodes.push_back(tonode);
			section.tracksplits[tonode] = clickedstate;
		}
		return section;
	}
	Vec posdiff = pos - frompos;
	float dir = atan2(-posdiff.y,posdiff.x);
	Node* fromnode = new Node(tracksystem, frompos, dir, -1);
	Tracksection newsection = Construction::extendtracktopos(tracksystem, fromnode, pos);
	newsection = newsection + Tracksection({},{fromnode});
	return newsection;
}

Tracksection buildat(Tracksystem& tracksystem, Node* fromnode, Vec pos)
{
	Tracksection section = planconstructionto(tracksystem, fromnode, pos);
	
	for(auto node : section.nodes)
		tracksystem.addnode(*node);
	for(auto track : section.tracks)
		tracksystem.addtrack(*track);
	for(auto [node, state] : section.tracksplits){
		Construction::splittrack(tracksystem, node, state);
	}

	return section;
}

Tracksection buildat(Tracksystem& tracksystem, Vec frompos, Vec topos)
{
	Tracksection section = planconstructionto(tracksystem, frompos, topos);
	
	for(auto node : section.nodes)
		tracksystem.addnode(*node);
	for(auto track : section.tracks)
		tracksystem.addtrack(*track);
	for(auto [node, state] : section.tracksplits){
		Construction::splittrack(tracksystem, node, state);
	}

	return section;
}

nodeid selectat(Tracksystem& tracksystem, Vec pos)
{
	nodeid selectednode = 0;
	whatdidiclick(tracksystem, pos, nullptr, &selectednode, nullptr, nullptr);
	return selectednode;
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
	trackid clickedtrack=0; signalid clickedsignal=0; switchid clickedswitch=0;
	State clickedstate = whatdidiclick(tracks, pos, &clickedtrack, nullptr, &clickedsignal, &clickedswitch);
	if(clickedtrack)
		neworder = new Gotostate(clickedstate);
	if(clickedsignal)
		neworder = new Setsignal(clickedsignal, tracks.getsignal(clickedsignal)->getcolorforordergeneration());
	if(clickedswitch)
		neworder = new Setswitch(clickedswitch, tracks.getswitch(clickedswitch)->getstateforordergeneration());
	return neworder;
}

void deleteat(Tracksystem& tracks, Vec pos)
{
	trackid clickedtrack=0; signalid clickedsignal=0;
	State clickedstate = whatdidiclick(tracks, pos, &clickedtrack, nullptr, &clickedsignal, nullptr);
	if(clickedtrack){
		bool mayremove = true;
		for(auto wagon: tracks.references->wagons){
			for(State* stateptr: wagon->getstates()){
				if(stateptr->track==clickedtrack)
					mayremove = false;
			}
		}
		if(mayremove){
			tracks.removetrack(clickedtrack);
		}
	}
	if(clickedsignal){
		Signal* signalptr = tracks.getsignal(clickedsignal);
		signalptr->disconnectfromtrack();
		tracks.removesignal(clickedsignal);
	}
	tracks.references->validatereferences();
}

float getcostoftracks(Tracksection section){
	float cost = 0;
	for(auto track : section.tracks)
		cost += 0.003*track->getarclength(1);
	return cost;
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