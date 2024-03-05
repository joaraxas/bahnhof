#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<string>
#include<map>
#include "utils.h"

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

Tracksystem::Tracksystem(ResourceManager& resources, std::vector<float> xs, std::vector<float> ys)
{
	allresources = &resources;
	nodeid newnode = addnode(Vec(xs[0], ys[0]), 0);
	for(int iNode = 1; iNode<xs.size(); iNode++){
		newnode = extendtracktopos(newnode, Vec(xs[iNode], ys[iNode]));
	}
	selectednode = newnode;
	switchtex = loadImage("assets/switch.png");
	SDL_QueryTexture(switchtex, NULL, NULL, &switchrect.w, &switchrect.h);
	switchrect.h = switchrect.h*0.5;
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
	else if(state.nodedist<0){
		Track* currenttrackpointer = gettrack(state.track);
		nodeid currentnode = currenttrackpointer->previousnode;
		float arclength1 = currenttrackpointer->getarclength(1);
		state.track = previoustrack(state.track);
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
			else
				distance += gettrack(state.track)->getarclength(1);
			if(abs(distance)>abs(maxdist)){
				finishedtrip = true;
				distance = maxdist;
			}
		}
	}

	return distance;
}

void Tracksystem::render()
{
	Vec mousepos(xMouse/scale+cam.x, yMouse/scale+cam.y);
	for(auto const& [id, track] : tracks)
		track->render();
	for(auto const& [id, node] : nodes)
		node->render();
	for(auto const& [id, signal] : signals)
		signal->render();
	
	if(selectednode){
		preparingtrack = true;
		nodeid lastnodeindex = nodecounter;
		trackid lasttrackindex = trackcounter;
		nodeid lastselectednode = selectednode;
		buildat(mousepos);
		selectednode = lastselectednode;
		for(trackid id = lasttrackindex+1; id<=trackcounter; id++)
			gettrack(id)->render();
		for(nodeid id = lastnodeindex+1; id<=nodecounter; id++)
			getnode(id)->render();
		for(trackid id = lasttrackindex+1; id<=trackcounter; id++)
			removetrack(id);
		for(nodeid id = lastnodeindex+1; id<=nodecounter; id++)
			removenode(id);
		nodecounter = lastnodeindex;
		trackcounter = lasttrackindex;
		preparingtrack = false;
	}
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
	if(mindist<20/scale){
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
	if(tracks.contains(track))
		return tracks[track];
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
	if(nexttrack == 0)
		nexttrack = track;
	return nexttrack;
}

trackid Tracksystem::previoustrack(trackid track){
	trackid previoustrack;
	Track* trackpointer = gettrack(track);
	if(trackpointer->isabovepreviousnode())
		previoustrack = getnode(trackpointer->previousnode)->gettrackdown();
	else
		previoustrack = getnode(trackpointer->previousnode)->gettrackup();
	if(previoustrack == 0)
		previoustrack = track;
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

bool Tracksystem::isred(State trainstate)
{
	bool red = false;
	for(auto const& [id, signal] : signals){
		if(signal->isred(trainstate))
			red = true;
		if(red)
			break;
	}
	return red;
}

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

void Node::render()
{
	if(!nicetracks){
		renderline(pos+Vec(-5,-5)/scale, pos+Vec(5,5)/scale);
		renderline(pos+Vec(-5,5)/scale, pos+Vec(5,-5)/scale);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		renderline(pos, pos+Vec(12*cos(dir),-12*sin(dir))/scale);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	}
	if(scale>0.3){
		if(size(tracksup)>1){
			Vec switchpos = getswitchpos(true);
			int diff = stateup/(size(tracksup)-1);
			int w = tracksystem->switchrect.w; int h = tracksystem->switchrect.h;
			SDL_Rect rect = {int(switchpos.x-w*0.5), int(switchpos.y-h*0.5), w, h};
			tracksystem->switchrect.y = tracksystem->switchrect.h*stateup;
			rendertexture(tracksystem->switchtex, &rect, &tracksystem->switchrect, dir-pi/2, true, true);
			if(!nicetracks)
				rendertext(std::to_string(stateup), switchpos.x, switchpos.y+7, {0,0,0,0});
		}
		if(size(tracksdown)>1){
			Vec switchpos = getswitchpos(false);
			int diff = statedown/(size(tracksdown)-1);
			int w = tracksystem->switchrect.w; int h = tracksystem->switchrect.h;
			SDL_Rect rect = {int(switchpos.x-w*0.5), int(switchpos.y-h*0.5), w, h};
			tracksystem->switchrect.y = tracksystem->switchrect.h*statedown;
			rendertexture(tracksystem->switchtex, &rect, &tracksystem->switchrect, pi+dir-pi/2, true, true);
			if(!nicetracks)
				rendertext(std::to_string(statedown), switchpos.x, switchpos.y+7, {0,0,0,0});
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

Track::Track(Tracksystem& newtracksystem, nodeid previous, nodeid next, trackid myid)
{
	previousnode = previous;
	nextnode = next;
	tracksystem = &newtracksystem;
	id = myid;

	previousdir = tracksystem->getnodedir(previousnode);
	previouspos = tracksystem->getnodepos(previousnode);
	nextdir = tracksystem->getnodedir(nextnode);
	nextpos = tracksystem->getnodepos(nextnode);
	float dx = cos(previousdir)*(nextpos.x - previouspos.x) - sin(previousdir)*(nextpos.y - previouspos.y);
	float dy = sin(previousdir)*(nextpos.x - previouspos.x) + cos(previousdir)*(nextpos.y - previouspos.y);
	radius = 0.5*(dy*dy+dx*dx)/dy;
	phi = sign(dy)*atan2(dx, sign(dy)*(radius-dy));
	if(abs(radius)>1e5){
		radius = INFINITY;
		phi = 0;
	}
}

Track::~Track()
{
	//std::cout << "deleted: " << id << std::endl;
	/*if(isrightofleftnode())
		nodeleft->tracksright.erase(find(nodeleft->tracksright.begin(),nodeleft->tracksright.end(),this));
	else
		nodeleft->tracksleft.erase(find(nodeleft->tracksleft.begin(),nodeleft->tracksleft.end(),this));
	nodeleft->incrementswitch();
	if(nodeleft->tracksleft.size()+nodeleft->tracksright.size() == 0)
		

	if(isleftofrightnode())
		noderight->tracksleft.erase(find(noderight->tracksleft.begin(),noderight->tracksleft.end(),this));
	else
		noderight->tracksright.erase(find(noderight->tracksright.begin(),noderight->tracksright.end(),this));
	noderight->incrementswitch();*/
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
			if(cos(tracksystem->getnodedir(previousnode)) > 0)
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

void Track::render()
{
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
		if(tracksystem->preparingtrack)
			SDL_SetRenderDrawColor(renderer, 255,255,255,127);
		float sleeperwidth = 2600/150;
		if(scale<0.2) sleeperwidth = 2600/150*0.25/scale;
		int nSleepers = round(getarclength(1)/3*fmin(1,scale));
		if(scale<0.3) nSleepers = round(getarclength(1)/20*fmin(1,scale));
		for(int iSleeper = 0; iSleeper < nSleepers; iSleeper++){
			float nodedist = float(iSleeper+0.5)/float(nSleepers);
			Vec drawposl = getpos(nodedist, sleeperwidth/2);
			Vec drawposr = getpos(nodedist, -sleeperwidth/2);{
				renderline(drawposl, drawposr);
			}
		}
	}
	//// rals ////
	if(nicetracks){
		SDL_SetRenderDrawColor(renderer, 0,0,0,255);
		if(tracksystem->preparingtrack)
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
		renderline(drawpos1l, drawpos2l);
	}
	if(gauge!=0)
	for(int iSegment = 0; iSegment < nSegments; iSegment++){
		float nodedist = float(iSegment)/float(nSegments);
		Vec drawpos1r = getpos(nodedist, -gauge/2);
		Vec drawpos2r = getpos(nodedist+1./nSegments, -gauge/2);
		renderline(drawpos1r, drawpos2r);
	}
	if(!nicetracks){
		Vec radiustextpos = getpos(0.5);
		std::string radiustext = std::to_string(int(round(radius*150*0.001))) + " m";
		if(isinf(radius))
			radiustext = std::to_string(radius);
		rendertext(radiustext, radiustextpos.x, radiustextpos.y, {255,255,255,255});
	}
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
}

Signal::Signal(Tracksystem& newtracksystem, State signalstate)
{
	tracksystem = &newtracksystem;
	state = signalstate;
	float orientation = tracksystem->getorientation(state);
	float transverseoffset = -20;
	pos = tracksystem->getpos(state) - Vec(sin(orientation), cos(orientation))*transverseoffset;
}

void Signal::render()
{
	SDL_SetRenderDrawColor(renderer, 255*(!isgreen), 255*(isgreen), 0, 255);
	//SDL_RenderDrawLine(renderer, pos.x-5, pos.y-5, pos.x+5, pos.y+5);
	//SDL_RenderDrawLine(renderer, pos.x-5, pos.y+5, pos.x+5, pos.y-5);
	renderline(pos+Vec(-5,-5), pos+Vec(5,5));
	renderline(pos+Vec(-5,5), pos+Vec(5,-5));
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

bool Signal::isred(State trainstate)
{
	bool red = false;
	if(!isgreen)
		if(tracksystem->distancefromto(trainstate, state, 100, true)<100)
			red = true;
	return red;
}