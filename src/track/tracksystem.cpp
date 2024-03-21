#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/input.h"
#include "bahnhof/common/camera.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/routing/routing.h"
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

Tracksystem::Tracksystem(){}

Tracksystem::Tracksystem(Game* whatgame, std::vector<float> xs, std::vector<float> ys)
{
	game = whatgame;
	nodeid newnode = addnode(Vec(xs[0], ys[0]), 0);
	for(int iNode = 1; iNode<xs.size(); iNode++){
		newnode = extendtracktopos(newnode, Vec(xs[iNode], ys[iNode]));
	}
	selectednode = 0;
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
	trackid newtrack = trackcounter;
	tracks[newtrack] = new Track(*this, previousnode, nextnode, newtrack);

	if(!preparingtrack){
			getnode(previousnode)->connecttrack(newtrack, gettrack(newtrack)->isabovepreviousnode());
			getnode(nextnode)->connecttrack(newtrack, !gettrack(newtrack)->isbelownextnode());
	}

	return newtrack;
}

signalid Tracksystem::addsignal(State state){
	signalcounter++;
	signals[signalcounter] = new Signal(*this, state);
	gettrack(state.track)->signals[state.nodedist] = signalcounter;
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

Vec Tracksystem::getpos(State state, float transverseoffset)
{
	gettrack(state.track);
	return gettrack(state.track)->getpos(state.nodedist, transverseoffset);
}

float Tracksystem::getorientation(State state)
{
	return gettrack(state.track)->getorientation(state.nodedist) + pi*!state.alignedwithtrack;
}

float Tracksystem::getradius(State state)
{
	Track* trackpointer = gettrack(state.track);
	float radd = trackpointer->radius*(2*state.alignedwithtrack-1)*(2*trackpointer->isabovepreviousnode()-1);
	return radd;
}

float Tracksystem::getradiusoriginatingfromnode(nodeid node, trackid track)
{
	Track* trackpointer = gettrack(track);
	State state(track, 0.5, trackpointer->previousnode==node);
	return getradius(state);
}

State Tracksystem::tryincrementingtrack(State state)
{
	if(state.nodedist>=1){
		Track* currenttrackpointer = gettrack(state.track);
		nodeid currentnode = currenttrackpointer->nextnode;
		float arclength1 = currenttrackpointer->getarclength(1);
		state.track = nexttrack(state.track);
		if(state.track){
			Track* nexttrackpointer = gettrack(state.track);
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
			state.track = currenttrackpointer->id;
			state.nodedist = 1;
		}
	}
	else if(state.nodedist<0){
		Track* currenttrackpointer = gettrack(state.track);
		nodeid currentnode = currenttrackpointer->previousnode;
		float arclength1 = currenttrackpointer->getarclength(1);
		state.track = previoustrack(state.track);
		if(state.track){
			Track* previoustrackpointer = gettrack(state.track);
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
			state.track = currenttrackpointer->id;
			state.nodedist = 0;
		}
	}
	return state;
}

State Tracksystem::travel(State state, float pixels)
{
	bool finishedtrip = false;

	float arclength1 = gettrack(state.track)->getarclength(1);
	state.nodedist += pixels*((state.alignedwithtrack)*2-1)/arclength1;

	while(!finishedtrip){
		trackid currenttrack = state.track;
		state = tryincrementingtrack(state);
		if(state.track==currenttrack) finishedtrip = true;
	}

	return state;
}

float Tracksystem::distancefromto(State state1, State state2, float maxdist, bool mustalign)
{
	State state = state1;
	float arclength = gettrack(state.track)->getarclength(1);
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
			state = tryincrementingtrack(state);
			if(state.track==state2.track && (!mustalign || state.alignedwithtrack==state2.alignedwithtrack)){
				finishedtrip = true;
				movingalongtrack = (maxdist>0)==state.alignedwithtrack;
				if(movingalongtrack)
					distance += gettrack(state.track)->getarclength(1)*state2.nodedist;
				else
					distance += gettrack(state.track)->getarclength(1)*(1-state2.nodedist);
			}
			else if(state.track==currenttrack){
				distance = maxdist;
			}
			else
				distance += gettrack(state.track)->getarclength(1);
			if(abs(distance)>=abs(maxdist)){
				finishedtrip = true;
				distance = maxdist;
			}
		}
	}

	return distance;
}

bool Tracksystem::isendofline(State state)
{
	if(state.nodedist<=0)
		if(!state.alignedwithtrack)
			if(previoustrack(state.track)==0)
				return true;
	if(state.nodedist>=1)
		if(state.alignedwithtrack)
			if(nexttrack(state.track)==0)
				return true;
	return false;
}

void Tracksystem::render(Rendering* r)
{
	for(auto const& [id, track] : tracks)
		track->render(r);
	for(auto const& [id, node] : nodes)
		node->render(r);
	
	if(selectednode){
		preparingtrack = true;
		nodeid lastnodeindex = nodecounter;
		trackid lasttrackindex = trackcounter;
		nodeid lastselectednode = selectednode;
		buildat(game->input->mapmousepos());
		selectednode = lastselectednode;
		for(trackid id = lasttrackindex+1; id<=trackcounter; id++)
			gettrack(id)->render(r);
		for(nodeid id = lastnodeindex+1; id<=nodecounter; id++)
			getnode(id)->render(r);
		for(trackid id = lasttrackindex+1; id<=trackcounter; id++)
			removetrack(id);
		for(nodeid id = lastnodeindex+1; id<=nodecounter; id++)
			removenode(id);
		nodecounter = lastnodeindex;
		trackcounter = lasttrackindex;
		preparingtrack = false;
	}
}

void Tracksystem::renderabovetrains(Rendering* r)
{
	for(auto const& [id, signal] : signals)
		signal->render(r);
}

float Tracksystem::buildat(Vec pos)
{
	float cost=0;
	if(placingsignal){
		trackid clickedtrack = 0;
		State signalstate = getcloseststate(pos);
		addsignal(signalstate);
		placingsignal = false;
		cost = 1;
	}
	else{
		trackid lasttrackindex = trackcounter;
		nodeid clickednode = 0;
		whatdidiclick(pos, nullptr, &clickednode, nullptr, nullptr);
		if(clickednode){
			connecttwonodes(selectednode, clickednode);
			selectednode = clickednode;
		}
		else
			selectednode = extendtracktopos(selectednode, pos);
		for(trackid id = lasttrackindex+1; id<=trackcounter; id++)
			cost += 0.003*gettrack(id)->getarclength(1);
	}
	return cost;
}

void Tracksystem::selectat(Vec pos)
{
	selectednode = 0;
	whatdidiclick(pos, nullptr, &selectednode, nullptr, nullptr);
}

bool Tracksystem::switchat(Vec pos)
{
	signalid clickedsignal=0; nodeid clickedswitch=0;
	State clickedstate = whatdidiclick(pos, nullptr, nullptr, &clickedsignal, &clickedswitch);
	if(clickedsignal)
		setsignal(clickedsignal, 2);
	if(clickedswitch)
		setswitch(clickedswitch, clickedstate.alignedwithtrack, -1);
	return(clickedsignal||clickedswitch);
}

Order* Tracksystem::generateorderat(Vec pos)
{
	Order* neworder = nullptr;
	trackid clickedtrack=0; signalid clickedsignal=0; nodeid clickedswitch=0;
	State clickedstate = whatdidiclick(pos, &clickedtrack, nullptr, &clickedsignal, &clickedswitch);
	if(clickedtrack)
		neworder = new Gotostate(clickedstate);
	if(clickedsignal)
		neworder = new Setsignal(clickedsignal, getsignalstate(clickedsignal));
	if(clickedswitch)
		neworder = new Setswitch(clickedswitch, clickedstate.alignedwithtrack, getswitchstate(clickedswitch, clickedstate.alignedwithtrack));
	return neworder;
}

State Tracksystem::whatdidiclick(Vec mousepos, trackid* track, nodeid* node, signalid* signal, nodeid* _switch)
{
	float trackdist = INFINITY, nodedist = INFINITY, signaldist = INFINITY, switchdist = INFINITY;
	State closeststate; nodeid closestnode = 0; signalid closestsignal = 0; nodeid closestswitch = 0;
	
	if(track){
		*track = 0;
		closeststate = getcloseststate(mousepos);
		if(closeststate.track)
			trackdist = distancetotrack(closeststate.track, mousepos);
	}
	if(node){
		*node = 0;
		closestnode = getclosestnode(mousepos);
		if(closestnode)
			nodedist = distancetonode(closestnode, mousepos);
	}
	if(signal){
		*signal = 0;
		closestsignal = getclosestsignal(mousepos);
		if(closestsignal)
			signaldist = distancetosignal(closestsignal, mousepos);
	}
	bool switchisup;
	if(_switch){
		*_switch = 0;
		closestswitch = getclosestswitch(mousepos, &switchisup);
		if(closestswitch)
			switchdist = distancetoswitch(closestswitch, mousepos, switchisup);
	}
	State returnstate;
	float mindist = std::min({trackdist, nodedist, signaldist, switchdist});
	if(mindist<20/game->cam->getscale()){
		if(trackdist==mindist){
			*track = closeststate.track;
			returnstate = closeststate;
		}
		else if(nodedist==mindist)
			*node = closestnode;
		else if(signaldist==mindist){
			*signal = closestsignal;
			returnstate = getsignal(closestsignal)->state;
		}
		else if(switchdist==mindist){
			*_switch = closestswitch;
			returnstate.alignedwithtrack = switchisup;
		}
	}
	return returnstate;
}

void Tracksystem::setswitch(nodeid node, bool updown, int switchstate)
{
	Node* nodeptr = getnode(node);
	if(switchstate==-1)
		nodeptr->incrementswitch(updown);
	else{
		if(updown)
			nodeptr->stateup = switchstate;
		else
			nodeptr->statedown = switchstate;
	}
}

int Tracksystem::getswitchstate(nodeid node, bool updown)
{
	Node* nodeptr = getnode(node);
	if(updown)
		return nodeptr->stateup;
	else
		return nodeptr->statedown;
}

void Tracksystem::setsignal(signalid signal, int redgreenorflip)
{
	Signal* signalpointer = getsignal(signal);
	if(redgreenorflip==2)
		signalpointer->isgreen = !signalpointer->isgreen;
	else
		signalpointer->isgreen = redgreenorflip;
}

bool Tracksystem::getsignalstate(signalid signal)
{
	return getsignal(signal)->isgreen;
}

void Tracksystem::deleteclick(int x, int y)
{
	selectednode = 0;
	Vec mousepos(x,y);
	nodeid clickednode = getclosestnode(mousepos);
	if(distancetonode(clickednode, mousepos)<=30){
		//removenode(clickednode);
	}
}

State Tracksystem::getcloseststate(Vec pos)
{
	float mindist = INFINITY;
	State closeststate;
	for(auto const& [id,track]: tracks){
		float dist = distancetotrack(id, pos);
		if(dist < mindist){
			closeststate = track->getcloseststate(pos);
			mindist = dist;
		}
	}
	return closeststate;
}

nodeid Tracksystem::getclosestnode(Vec pos)
{
	float mindistsquared = INFINITY;
	nodeid closestnode = 0;
	for(auto const& [id,node]: nodes){
		float distsquared = pow(node->pos.x-pos.x, 2) + pow(node->pos.y-pos.y, 2);
		if(distsquared < mindistsquared){
			closestnode = id;
			mindistsquared = distsquared;
		}
	}
	return closestnode;
}

signalid Tracksystem::getclosestsignal(Vec pos)
{
	float mindistsquared = INFINITY;
	signalid closestsignal = 0;
	for(auto const& [id,signal]: signals){
		float distsquared = pow(signal->pos.x-pos.x, 2) + pow(signal->pos.y-pos.y, 2);
		if(distsquared < mindistsquared){
			closestsignal = id;
			mindistsquared = distsquared;
		}
	}
	return closestsignal;
}

nodeid Tracksystem::getclosestswitch(Vec pos, bool* updown)
{
	float mindist = INFINITY;
	nodeid closestswitch = 0;
	for(auto const& [id,node]: nodes){
		if(size(node->tracksup)>1){
			float dist = distancetoswitch(id, pos, true);
			if(dist < mindist){
				closestswitch = id;
				mindist = dist;
				*updown = true;
			}
		}
		if(size(node->tracksdown)>1){
			float dist = distancetoswitch(id, pos, false);
			if(dist < mindist){
				closestswitch = id;
				mindist = dist;
				*updown = false;
			}
		}
	}
	return closestswitch;
}

nodeid Tracksystem::extendtracktopos(nodeid fromnode, Vec pos)
{
	Vec posdiff = pos - getnodepos(fromnode);
	float dir = truncate(2*atan2(-posdiff.y,posdiff.x) - getnodedir(fromnode));
	nodeid newnode = addnode(pos, dir);
	addtrack(fromnode, newnode);
	return newnode;
}

void Tracksystem::connecttwonodes(nodeid node1, nodeid node2)
{
	if(node1==node2)
		return;
	//for(auto& track : node1->tracksleft)
	//	if(track->nodeleft==node2 || track->noderight==node2)
	//		return;
	//for(auto& track : node1->tracksright)
	//	if(track->nodeleft==node2 || track->noderight==node2)
	//		return;
	Vec newnodepoint;
	float y1 = -getnodepos(node1).y;
	float y2 = -getnodepos(node2).y;
	float x1 = getnodepos(node1).x;
	float x2 = getnodepos(node2).x;
	float tanth1 = tan(getnodedir(node1));
	float tanth2 = tan(getnodedir(node2));
	float intersectx = (y2-y1+x1*tanth1 - x2*tanth2)/(tanth1 - tanth2);
	float intersecty = -(y1 + (intersectx - x1)*tanth1);
	if(abs(tanth1)>1e5){
		intersectx = x1;
		intersecty = -(y2 + (intersectx - x2)*tanth2);
	}
	Vec tangentintersection(intersectx, intersecty);
	float disttointersect1 = distancetonode(node1, tangentintersection);
	float disttointersect2 = distancetonode(node2, tangentintersection);
	if(disttointersect1 > disttointersect2)
		newnodepoint = tangentintersection + (getnodepos(node1) - tangentintersection)/disttointersect1*disttointersect2;
	else
		newnodepoint = tangentintersection + (getnodepos(node2) - tangentintersection)/disttointersect2*disttointersect1;
	if(distancetonode(node1, newnodepoint)> 10 && distancetonode(node2, newnodepoint)> 10){ //TODO: bug when connecting two nodes where one is in plane of other but directions not parallel
		nodeid newnode = extendtracktopos(node1, newnodepoint);
		addtrack(newnode, node2);
	}
	else
		addtrack(node1, node2);
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

float Tracksystem::getnodedir(nodeid node)
{
	return getnode(node)->dir;
}

Vec Tracksystem::getnodepos(nodeid node)
{
	return getnode(node)->pos;
}

Vec Tracksystem::getsignalpos(signalid signal)
{
	return getsignal(signal)->pos;
}

Vec Tracksystem::getswitchpos(nodeid node, bool updown)
{
	return getnode(node)->getswitchpos(updown);
}

float Tracksystem::distancetotrack(trackid track, Vec pos)
{
	return norm(getpos(gettrack(track)->getcloseststate(pos))-pos);
}

float Tracksystem::distancetonode(nodeid node, Vec pos)
{
	return norm(getnodepos(node)-pos);
}

float Tracksystem::distancetosignal(signalid signal, Vec pos)
{
	return norm(getsignalpos(signal)-pos);
}

float Tracksystem::distancetoswitch(nodeid node, Vec pos, bool updown)
{
	return norm(getswitchpos(node, updown)-pos);
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

trackid Tracksystem::nexttrack(trackid track){
	trackid nexttrack;
	Track* trackpointer = gettrack(track);
	if(trackpointer->isbelownextnode()){
		nexttrack = getnode(trackpointer->nextnode)->gettrackup();
	}
	else
		nexttrack = getnode(trackpointer->nextnode)->gettrackdown();
	//if(nexttrack == 0)
	//	nexttrack = track;
	return nexttrack;
}

trackid Tracksystem::previoustrack(trackid track){
	trackid previoustrack;
	Track* trackpointer = gettrack(track);
	if(trackpointer->isabovepreviousnode())
		previoustrack = getnode(trackpointer->previousnode)->gettrackdown();
	else
		previoustrack = getnode(trackpointer->previousnode)->gettrackup();
	//if(previoustrack == 0)
	//	previoustrack = track;
	return previoustrack;
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

bool Tracksystem::isred(Train* fortrain)
{
	bool red = false;
	for(auto const& [id, signal] : signals){
		if(signal->isred(fortrain))
			red = true;
		if(red)
			break;
	}
	return red;
}

signalid Tracksystem::nextsignalontrack(State state, bool startfromtrackend, bool mustalign)
{
	if(startfromtrackend) state.nodedist = 1-state.alignedwithtrack;
	signalid reachedsignal = 0;
	Track* trackpointer = gettrack(state.track);
	if(state.alignedwithtrack){
		for(auto const& [nodedist,signal]: trackpointer->signals){
			if(nodedist>state.nodedist){
				if(getsignal(signal)->state.alignedwithtrack || !mustalign){
					reachedsignal = signal;
				}
			}
			if(reachedsignal) break;
		}
	}
	else{
		for(auto const& [nodedist,signal]: trackpointer->signals){
			if(nodedist<state.nodedist)
				if(!getsignal(signal)->state.alignedwithtrack || !mustalign)
					reachedsignal = signal;
		}
	}
	return reachedsignal;
}

void Tracksystem::setblocksuptonextsignal(Signal* fromsignal)
{
	bool reachedend = false;
	fromsignal->switchblocks.clear();
	fromsignal->signalblocks.clear();
	State goalstate = fromsignal->state;
	signalid reachedsignal = nextsignalontrack(goalstate);
	goalstate.nodedist = (2*goalstate.alignedwithtrack-1)*INFINITY;
	nodeid passednode = 0;
	while(!reachedsignal && !reachedend){
		if(goalstate.alignedwithtrack)
			passednode = gettrack(goalstate.track)->nextnode;
		else
			passednode = gettrack(goalstate.track)->previousnode;
		if(getnode(passednode)->tracksup.size()>1 || getnode(passednode)->tracksdown.size()>1){
			if(std::find(fromsignal->switchblocks.begin(), fromsignal->switchblocks.end(), passednode) != fromsignal->switchblocks.end()){
				reachedend = true;
			}
			else{
				fromsignal->switchblocks.push_back(passednode);
			}
		}
		State newgoalstate = tryincrementingtrack(goalstate);
		if(newgoalstate.track==goalstate.track)
			reachedend = true;
		else{
			reachedsignal = nextsignalontrack(newgoalstate, true);
		}
		goalstate = newgoalstate;
	}
	if(reachedsignal)
		fromsignal->signalblocks.push_back(reachedsignal);
}

bool Tracksystem::checkblocks(std::vector<nodeid> switchblocks, std::vector<signalid> signalblocks, Train* fortrain)
{
	for(auto s : switchblocks){
		Node* sw = getnode(s);
		if(sw->reservedfor && sw->reservedfor!=fortrain)
			return false;
	}
	for(auto s : signalblocks){
		Signal* si = getsignal(s);
		if(si->reservedfor && si->reservedfor!=fortrain)
			return false;
	}
	return true;
}

bool Tracksystem::claimblocks(std::vector<nodeid> switchblocks, std::vector<signalid> signalblocks, Train* fortrain)
{
	if(checkblocks(switchblocks, signalblocks, fortrain)){
		for(auto s : switchblocks){
			Node* sw = getnode(s);
			sw->reservedfor = fortrain;
		}
		for(auto s : signalblocks){
			Signal* si = getsignal(s);
			si->reservedfor = fortrain;
		}
		return true;
	}
	else return false;
}

void Tracksystem::update(int ms)
{
	for(auto const& [id, signal] : signals){
    	setblocksuptonextsignal(signal);
		if(checkblocks(signal->switchblocks, signal->signalblocks, nullptr))
			signal->isgreen = true;
		else
			signal->isgreen = false;
	}
}

void Tracksystem::runoverblocks(State state, float pixels, Train* fortrain)
{
	signalid nextsignal = nextsignalontrack(state, false, false);
	if(nextsignal){
		Signal* signalpointer = getsignal(nextsignal);
		if(distancefromto(state, signalpointer->state, pixels)<pixels)
			signalpointer->reservedfor = fortrain;
	}
	State newstate = travel(state, pixels);
	if(newstate.track==state.track)
		return;
	for(auto const& [id, node] : nodes){
		if(node->tracksdown.size()>1 || node->tracksup.size()>1){
			if(gettrack(state.track)->previousnode==id || gettrack(state.track)->nextnode==id)
			if(gettrack(newstate.track)->previousnode==id || gettrack(newstate.track)->nextnode==id)
				node->reservedfor=fortrain;
		}
	}
}