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

Tracksystem::Tracksystem(){}

Tracksystem::Tracksystem(ResourceManager& resources, std::vector<float> xs, std::vector<float> ys)
{
	allresources = &resources;
	nodeid newnode = addnode(Vec(xs[0], ys[0]), 0);
	for(int iNode = 1; iNode<xs.size(); iNode++){
		newnode = extendtracktopos(newnode, Vec(xs[iNode], ys[iNode]));
	}
	selectednode = newnode;
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
		if(gettrack(newtrack)->isabovepreviousnode())
			getnode(previousnode)->tracksup.push_back(newtrack);
		else
			getnode(previousnode)->tracksdown.push_back(newtrack);
		if(gettrack(newtrack)->isbelownextnode())
			getnode(nextnode)->tracksdown.push_back(newtrack);
		else
			getnode(nextnode)->tracksup.push_back(newtrack);
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

State Tracksystem::travel(State state, float pixels)
{
	trackid oldtrack = state.track;
	trackid newtrack = oldtrack;
	float nodedist = state.nodedist;
	bool alignedwithtrack = state.alignedwithtrack;
	bool finishedtrip = false;

	float arclength1 = gettrack(oldtrack)->getarclength(1);
	float arclength2 = 0;
	nodedist += pixels*((alignedwithtrack)*2-1)/arclength1;

	while(!finishedtrip){
		if(nodedist>=1)
		{
			nodeid currentnode = gettrack(oldtrack)->nextnode;
			newtrack = nexttrack(oldtrack);
			arclength2 = gettrack(newtrack)->getarclength(1);
			if(gettrack(newtrack)->previousnode==currentnode){
				nodedist = (nodedist-1)*arclength1/arclength2;
			}
			else{
				nodedist = 1-(nodedist-1)*arclength1/arclength2;
				alignedwithtrack = !alignedwithtrack;
			}
		}
		else if(nodedist<0)
		{
			nodeid currentnode = gettrack(oldtrack)->previousnode;
			newtrack = previoustrack(oldtrack);
			arclength2 = gettrack(newtrack)->getarclength(1);
			if(gettrack(newtrack)->nextnode==currentnode){
				nodedist = 1-(-nodedist)*arclength1/arclength2;
			}
			else{
				nodedist = (-nodedist)*arclength1/arclength2;
				alignedwithtrack = !alignedwithtrack;
			}
		}
		else finishedtrip = true;
		oldtrack = newtrack;
		arclength1 = arclength2;
	}

	return State(oldtrack, nodedist, alignedwithtrack);
}

void Tracksystem::render()
{
	for(auto const& [id, track] : tracks)
		track->render();
	for(auto const& [id, node] : nodes)
		node->render();
	for(auto const& [id, signal] : signals)
		signal->render();
	
	preparingtrack = true;
	nodeid lastnodeindex = nodecounter;
	trackid lasttrackindex = trackcounter;

	Vec mousepos(xMouse, yMouse);
	nodeid snappednode = getclosestnode(mousepos);
	bool snappedextantnode = distancetonode(snappednode, mousepos)<=20;
	if(selectednode){
		if(snappedextantnode){
			connecttwonodes(selectednode, snappednode);
		}
		else{
			nodeid newnode = extendtracktopos(selectednode, mousepos);
		}
	}

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

void Tracksystem::leftclick(int x, int y)
{
	Vec mousepos(x, y);
	nodeid clickednode = getclosestnode(mousepos);
	bool clickedextantnode = distancetonode(clickednode, mousepos)<=20;
	if(!selectednode){
		if(clickedextantnode)
			selectednode = clickednode;
	}
	else{
		if(clickedextantnode){
			connecttwonodes(selectednode, clickednode);
			selectednode = clickednode;
		}
		else{
			nodeid newnode = extendtracktopos(selectednode, mousepos);
			selectednode = newnode;
		}
	}
}

void Tracksystem::rightclick(int x, int y)
{
	selectednode = 0;
	Vec mousepos(x,y);
	trackid clickedtrack=0; nodeid clickednode=0; signalid clickedsignal=1; nodeid clickedswitch=1;
	State clickedstate = whatdidiclick(mousepos, &clickedtrack, &clickednode, &clickedsignal, &clickedswitch);
	if(clickedsignal)
		setsignal(clickedsignal, 2);
	if(clickedswitch)
		setswitch(clickedswitch, clickedstate.alignedwithtrack, -1);
}

State Tracksystem::whatdidiclick(Vec mousepos, trackid* track, nodeid* node, signalid* signal, nodeid* _switch)
{
	float trackdist = INFINITY, nodedist = INFINITY, signaldist = INFINITY, switchdist = INFINITY;
	if(*track){
		//track = getclosestnode(mousepos)
	}
	if(*node){
		*node = getclosestnode(mousepos);
		if(*node)
			nodedist = distancetonode(*node, mousepos);
	}
	if(*signal){
		*signal = getclosestsignal(mousepos);
		if(*signal)
			signaldist = distancetosignal(*signal, mousepos);
	}
	bool switchisup;
	if(*_switch){
		*_switch = getclosestswitch(mousepos, &switchisup);
		if(*_switch)
			switchdist = distancetoswitch(*_switch, mousepos, switchisup);
	}
	State returnstate;
	float mindist = std::min({trackdist, nodedist, signaldist, switchdist});
	if(mindist<20){
		if(trackdist==mindist){
			*node = 0, *signal = 0, *_switch = 0;
		}
		else if(nodedist==mindist){
			*track = 0, *signal = 0, *_switch = 0;
		}
		else if(signaldist==mindist){
			*track = 0, *node = 0, *_switch = 0;
			returnstate = getsignal(*signal)->state;
		}
		else if(switchdist==mindist){
			*track = 0, *node = 0, *signal = 0;
			returnstate.alignedwithtrack = switchisup;
		}
	}
	else
		*track = 0, *node = 0, *signal = 0, *_switch = 0;
	return returnstate;
}

int Tracksystem::setswitch(nodeid node, bool updown, int switchstate)
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

bool Tracksystem::setsignal(signalid signal, int redgreenorflip)
{
	Signal* signalpointer = getsignal(signal);
	if(redgreenorflip==2)
		signalpointer->isgreen = !signalpointer->isgreen;
	else
		signalpointer->isgreen = redgreenorflip;
	return signalpointer->isgreen;
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

bool Tracksystem::isred(State trainstate, float pixels)
{
	bool red = false;
	for(auto const& [id, signal] : signals)
		if(signal->isred(trainstate, pixels)){
			red = true;
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

void Node::render()
{
	if(!nicetracks){
		SDL_RenderDrawLine(renderer, pos.x-5, pos.y-5, pos.x+5, pos.y+5);
		SDL_RenderDrawLine(renderer, pos.x-5, pos.y+5, pos.x+5, pos.y-5);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderDrawLine(renderer, pos.x, pos.y, pos.x+12*cos(dir), pos.y-12*sin(dir));
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	}
	if(size(tracksup)>1){
		Vec switchpos = getswitchpos(true);
		if(stateup>0)
			SDL_RenderDrawLine(renderer, switchpos.x-5, switchpos.y, switchpos.x+5, switchpos.y);
		else
			SDL_RenderDrawLine(renderer, switchpos.x, switchpos.y-5, switchpos.x, switchpos.y+5);
	}
	if(size(tracksdown)>1){
		Vec switchpos = getswitchpos(false);
		if(statedown>0)
			SDL_RenderDrawLine(renderer, switchpos.x-5, switchpos.y, switchpos.x+5, switchpos.y);
		else
			SDL_RenderDrawLine(renderer, switchpos.x, switchpos.y-5, switchpos.x, switchpos.y+5);
	}
}

Vec Node::getswitchpos(bool updown)
{
	float transverseoffset = -(2*updown-1)*15;
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
	//// badd ////
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
		float sleeperwidth = 2600/150/scale;
		int nSleepers = round(getarclength(1)/3);
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
	if(nicetracks) gauge = 1435/150/scale;
	for(int iSegment = 0; iSegment < nSegments; iSegment++){
		float nodedist = float(iSegment)/float(nSegments);
		Vec drawpos1l = getpos(nodedist, gauge/2);
		Vec drawpos2l = getpos(nodedist+1./nSegments, gauge/2);
		Vec drawpos1r = getpos(nodedist, -gauge/2);
		Vec drawpos2r = getpos(nodedist+1./nSegments, -gauge/2);
		if(drawpos2l.x>0)
		if(drawpos2l.x<SCREEN_WIDTH)
		if(drawpos2l.y>0)
		if(drawpos2l.y<SCREEN_HEIGHT){
			SDL_RenderDrawLine(renderer, drawpos1l.x, drawpos1l.y, drawpos2l.x, drawpos2l.y);
			SDL_RenderDrawLine(renderer, drawpos1r.x, drawpos1r.y, drawpos2r.x, drawpos2r.y);
		}
	}
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
}

Signal::Signal(Tracksystem& newtracksystem, State signalstate)
{
	tracksystem = &newtracksystem;
	state = signalstate;
	float orientation = tracksystem->getorientation(state)+pi*state.alignedwithtrack;
	float transverseoffset = -20/scale;
	pos = tracksystem->getpos(state) - Vec(sin(orientation), cos(orientation))*transverseoffset;
}

void Signal::render()
{
	SDL_SetRenderDrawColor(renderer, 255*(!isgreen), 255*(isgreen), 0, 255);
	SDL_RenderDrawLine(renderer, pos.x-5, pos.y-5, pos.x+5, pos.y+5);
	SDL_RenderDrawLine(renderer, pos.x-5, pos.y+5, pos.x+5, pos.y-5);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

bool Signal::isred(State trainstate, float pixels)
{
	bool red = false;
	if(!isgreen){
		if(trainstate.track == state.track)
			if((trainstate.alignedwithtrack==(pixels>0)) == state.alignedwithtrack){
				State backwardstate = tracksystem->travel(state, -sign(pixels)*60);
				if((trainstate.nodedist > state.nodedist) != state.alignedwithtrack)
				if((trainstate.nodedist < backwardstate.nodedist) != backwardstate.alignedwithtrack)
					red = true;
			}
	}
	/*else{
		State forwardstate = tracksystem->travel(state, sign(pixels)*60);
		if(trainstate.track == forwardstate.track){
			if((trainstate.alignedwithtrack==(pixels>0)) == forwardstate.alignedwithtrack)
				if((trainstate.nodedist < state.nodedist) != state.alignedwithtrack)
					isgreen = false;
		}
	}*/
	return red;
}