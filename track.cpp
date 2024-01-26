#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<string>
#include<map>
#include "utils.h"

Tracksystem::Tracksystem(ResourceManager& resources, std::vector<float> xs, std::vector<float> ys)
{
	allresources = &resources;
	nodeid newnode = addnode(Vec(xs[0], ys[0]), 0);
	for(int iNode = 1; iNode<xs.size(); iNode++){
		newnode = extendtracktopos(newnode, Vec(xs[iNode], ys[iNode]));
	}
	selectednode = newnode;
}

nodeid Tracksystem::addnode(Vec pos, float dir){
	nodecounter++;
	nodes[nodecounter] = new Node(*this, pos, dir);
	return nodecounter;
}

trackid Tracksystem::addtrack(nodeid previousnode, nodeid nextnode){
	trackcounter++;
	tracks[trackcounter] = new Track(*this, previousnode, nextnode, trackcounter);
	return trackcounter;
}

void Tracksystem::removenode(nodeid nodetoremove)
{
	delete nodes[nodetoremove];
	nodes.erase(nodetoremove);
}

void Tracksystem::removetrack(nodeid tracktoremove)
{
	delete tracks[tracktoremove];
	tracks.erase(tracktoremove);
}

Vec Tracksystem::getpos(trackid track, float nodedist)
{
	return gettrack(track)->getpos(nodedist);
}

float Tracksystem::getorientation(trackid track, float nodedist, bool alignedwithtrackdirection)
{
	return gettrack(track)->getorientation(nodedist) + pi*!alignedwithtrackdirection;
}

void Tracksystem::travel(trackid* trackpointer, float* nodedistpointer, bool* alignedwithtrackpointer, float pixels)
{
	trackid oldtrack = *trackpointer;
	trackid newtrack = oldtrack;
	float nodedist = *nodedistpointer;
	bool alignedwithtrack = *alignedwithtrackpointer;
	bool finishedtrip = false;

	float arclength1 = gettrack(oldtrack)->getarclength(1);
	float arclength2 = 0;
	nodedist += pixels*((alignedwithtrack)*2-1)/arclength1;

	while(!finishedtrip){
		if(nodedist>=1)
		{
			nodeid currentnode = gettrack(oldtrack)->nextnode;
			newtrack = gettrack(oldtrack)->getnexttrack();
			arclength2 = gettrack(newtrack)->getarclength(1);
			std::cout<<newtrack<<std::endl;
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
			newtrack = gettrack(oldtrack)->getprevioustrack();
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

	*trackpointer = oldtrack;
	*nodedistpointer = nodedist;
	*alignedwithtrackpointer = alignedwithtrack;
}

void Tracksystem::render()
{
	for(auto const& [id, track] : tracks)
		track->render();
	for(auto const& [id, node] : nodes)
		node->render();
}

void Tracksystem::leftclick(int xMouse, int yMouse)
{
	Vec mousepos(xMouse, yMouse);
	nodeid clickednode = getclosestnode(mousepos);
	float mousedistance = norm(getnodepos(clickednode) - mousepos);
	bool clickedextantnode = mousedistance<=20;
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

void Tracksystem::rightclick(int xMouse, int yMouse)
{
	selectednode = 0;
	Vec mousepos(xMouse,yMouse);
	nodeid clickednode = getclosestnode(mousepos);
	if(distancetonode(clickednode, mousepos)<=30){
		getnode(clickednode)->incrementswitch();
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
	if(distancetonode(node1, newnodepoint)> 10 && distancetonode(node2, newnodepoint)> 10){
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

float Tracksystem::distancetonode(nodeid node, Vec pos)
{
	return norm(getnodepos(node)-pos);
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

Node::Node(Tracksystem& newtracksystem, Vec posstart, float dirstart)
{
	tracksystem = &newtracksystem;
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
}

void Node::connecttrackfromabove(trackid track)
{
	tracksup.push_back(track);
}

void Node::connecttrackfrombelow(trackid track)
{
	tracksdown.push_back(track);
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

void Node::incrementswitch()
{
	statedown++;
	stateup++;
	if(stateup>=tracksup.size())
		stateup = 0;
	if(statedown>=tracksdown.size())
		statedown = 0;
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

	if(isabovepreviousnode())
		tracksystem->getnode(previousnode)->connecttrackfromabove(id);
	else
		tracksystem->getnode(previousnode)->connecttrackfrombelow(id);
	if(isbelownextnode())
		tracksystem->getnode(nextnode)->connecttrackfrombelow(id);
	else
		tracksystem->getnode(nextnode)->connecttrackfromabove(id);
}

Track::~Track()
{
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

trackid Track::getnexttrack(){
	trackid nexttrack;
	std::cout << getorientation(1) << std::endl;
	std::cout << nextdir << std::endl;
	if(isbelownextnode()){
		std::cout<<"below"<<std::endl;
		nexttrack = tracksystem->getnode(nextnode)->gettrackup();
	}
	else
		nexttrack = tracksystem->getnode(nextnode)->gettrackdown();
	if(nexttrack == 0)
		nexttrack = id;
	return nexttrack;
}

trackid Track::getprevioustrack(){
	trackid previoustrack;
	if(isabovepreviousnode())
		previoustrack = tracksystem->getnode(previousnode)->gettrackdown();
	else
		previoustrack = tracksystem->getnode(previousnode)->gettrackup();
	if(previoustrack == 0)
		previoustrack = id;
	return previoustrack;
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
	if(nicetracks) SDL_SetRenderDrawColor(renderer, 0,0,0,255);
	else SDL_SetRenderDrawColor(renderer, 255*isabovepreviousnode(),0, 255*isbelownextnode(),255);
	int nSegments = 1;
	if(!isinf(radius))
		nSegments = fmax(1,round(abs(phi/2/pi*4*16)));
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

