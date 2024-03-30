#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/camera.h"
#include "bahnhof/common/input.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/trackinternal.h"
#include "bahnhof/track/track.h"

State::State()
{
	track = 0; 
	nodedist = 0; 
	alignedwithtrack = true;
}

State::State(trackid trackstart, float nodediststart, bool alignedwithtrackstart)
{
	track = trackstart; 
	nodedist = nodediststart; 
	alignedwithtrack = alignedwithtrackstart;
}

State flipstate(State state)
{
	State outstate = state;
	outstate.alignedwithtrack = !outstate.alignedwithtrack;
	return outstate;
}

namespace Tracks{

Tracksystem::Tracksystem(Game& whatgame, std::vector<float> xs, std::vector<float> ys)
{
	game = &whatgame;
	nodeid newnode = addnode(Vec(xs[0], ys[0]), 0);
	for(int iNode = 1; iNode<xs.size(); iNode++){
		newnode = Construction::extendtracktopos(*this, newnode, Vec(xs[iNode], ys[iNode]));
	}
	//selectednode = 0;
}

Tracksystem::~Tracksystem()
{
	auto trackscopy = tracks;
	for(auto & [id, track] : trackscopy)
		removetrack(id);
	auto nodescopy = nodes;
	for(auto & [id, node] : nodescopy)
		removenode(id);
	auto signalscopy = signals;
	for(auto & [id, signal] : signalscopy)
		removesignal(id);
}

nodeid Tracksystem::addnode(Vec pos, float dir){
	nodecounter++;
	nodes[nodecounter] = new Node(*this, pos, dir, nodecounter);
	return nodecounter;
}

trackid Tracksystem::addtrack(nodeid previousnode, nodeid nextnode){
	trackcounter++;
	tracks[trackcounter] = new Track(*this, *getnode(previousnode), *getnode(nextnode), trackcounter);
	return trackcounter;
}

switchid Tracksystem::addswitchtolist(Switch* _switch){
	switchcounter++;
	switches[switchcounter] = _switch;
	return switchcounter;
}

signalid Tracksystem::addsignal(State state){
	signalcounter++;
	signals[signalcounter] = new Signal(*this, state, signalcounter);
	gettrack(state.track)->addsignal(state, signalcounter);
	return signalcounter;
}

void Tracksystem::removenode(nodeid toremove)
{
	delete nodes[toremove];
	nodes.erase(toremove);
}

void Tracksystem::removetrack(trackid toremove)
{
	delete tracks[toremove];
	tracks.erase(toremove);
}

void Tracksystem::removesignal(signalid toremove)
{
	delete signals[toremove];
	signals.erase(toremove);
}



void render(Tracksystem& tracksystem, Rendering* r)
{
	for(auto const track : tracksystem.alltracks())
		track->render(r, 0);
	for(auto const node : tracksystem.allnodes())
		node->render(r);
	
	if(tracksystem.selectednode){
		tracksystem.preparingtrack = true;
		nodeid lastnodeindex = tracksystem.nodecounter;
		trackid lasttrackindex = tracksystem.trackcounter;
		nodeid lastselectednode = tracksystem.selectednode;
		Input::buildat(tracksystem, tracksystem.game->getinputmanager().mapmousepos());
		tracksystem.selectednode = lastselectednode;
		for(trackid id = lasttrackindex+1; id<=tracksystem.trackcounter; id++)
			tracksystem.gettrack(id)->render(r, 1);
		for(nodeid id = lastnodeindex+1; id<=tracksystem.nodecounter; id++)
			tracksystem.getnode(id)->render(r);
		for(trackid id = lasttrackindex+1; id<=tracksystem.trackcounter; id++)
			tracksystem.removetrack(id);
		for(nodeid id = lastnodeindex+1; id<=tracksystem.nodecounter; id++)
			tracksystem.removenode(id);
		tracksystem.nodecounter = lastnodeindex;
		tracksystem.trackcounter = lasttrackindex;
		tracksystem.preparingtrack = false;
	}
}

void renderabovetrains(Tracksystem& tracksystem, Rendering* r)
{
	for(auto const& _switch : tracksystem.allswitches())
		_switch->render(r);
	for(auto const& signal : tracksystem.allsignals())
		signal->render(r);
}



Node* Tracksystem::getnode(nodeid node)
{
	if(nodes.contains(node))
		return nodes[node];
	else{
		std::cout << "Error: failed to find node with id" << node << std::endl;
		return nullptr;
	}
}

std::vector<Track*> Tracksystem::alltracks()
{
	std::vector<Track*> trackrefs;
	for(auto const& [id, track] : tracks)
    	trackrefs.push_back(track);
	return trackrefs;
}

std::vector<Node*> Tracksystem::allnodes()
{
	std::vector<Node*> noderefs;
	for(auto const& [id, node] : nodes)
    	noderefs.push_back(node);
	return noderefs;
}

std::vector<Switch*> Tracksystem::allswitches()
{
	std::vector<Switch*> refs;
	for(auto const& [id, _switch] : switches)
    	refs.push_back(_switch);
	return refs;
}

std::vector<Signal*> Tracksystem::allsignals()
{
	std::vector<Signal*> refs;
	for(auto const& [id, signal] : signals)
    	refs.push_back(signal);
	return refs;
}

Switch* Tracksystem::getswitch(switchid _switch)
{
	if(switches.contains(_switch))
		return switches[_switch];
	else{
		std::cout << "Error: failed to find switch with id" << _switch << std::endl;
		return nullptr;
	}
}

float distancebetween(Vec pos1, Vec pos2)
{
	return norm(pos1 - pos2);
}

Track* Tracksystem::gettrack(trackid track)
{
	if(tracks.contains(track)){
		return tracks[track];
	}
	else{
		std::cout << "Error: failed to find track with id" << track << std::endl;
		return nullptr;
	}
}

Signal* Tracksystem::getsignal(signalid signal)
{
	if(signals.contains(signal))
		return signals[signal];
	else{
		std::cout << "Error: failed to find signal with id" << signal << std::endl;
		return nullptr;
	}
}
}