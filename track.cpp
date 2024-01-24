#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<string>
#include<map>
#include "utils.h"

Tracksystem::Tracksystem(ResourceManager& resources, std::vector<float> xs, std::vector<float> ys)
{
	allresources = &resources;
	Node* newnode = addnode(xs[0], ys[0], 0);
	for(int iNode = 1; iNode<xs.size(); iNode++){
		newnode = extendtrackto(newnode, Vec(xs[iNode], ys[iNode]));
	}
	selectednode = newnode;
}

Node* Tracksystem::addnode(float x, float y, float dir){
	nodes.push_back(std::unique_ptr<Node>{new Node(x, y, dir)});
	return nodes.back().get();
}

Track* Tracksystem::addtrack(Node* leftnode, Node* rightnode){
	tracks.push_back(std::unique_ptr<Track>{new Track(leftnode, rightnode)});
	return tracks.back().get();
}

void Tracksystem::render()
{
	for(int iTrack=0; iTrack<tracks.size(); iTrack++){
		tracks[iTrack]->render();
	}
	for(auto& node: nodes){
		node->render();
	}
}

void Tracksystem::leftclick(int xMouse, int yMouse)
{
	Vec mousepos(xMouse, yMouse);
	Node* clickednode = getclosestnode(mousepos);
	float mousedistance = norm(clickednode->pos - mousepos);
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
			Node* newnode = extendtrackto(selectednode, mousepos);
			selectednode = newnode;
		}
	}
}

void Tracksystem::rightclick(int xMouse, int yMouse)
{
	selectednode = nullptr;
	Vec mousepos(xMouse,yMouse);
	Node* clickednode = getclosestnode(mousepos);
	if(norm(clickednode->pos-mousepos)<=30){
		clickednode->incrementswitch();
	}
}

Node* Tracksystem::getclosestnode(Vec pos)
{
	float mindistsquared = INFINITY;
	Node* closestnode = nullptr;
	for(auto& node: nodes){
		float distsquared = pow(node->pos.x-pos.x, 2) + pow(node->pos.y-pos.y, 2);
		if(distsquared < mindistsquared){
			closestnode = node.get();
			mindistsquared = distsquared;
		}
	}
	return closestnode;
}

Node* Tracksystem::extendtrackto(Node* fromnode, Vec pos)
{
	Vec posdiff = pos - fromnode->pos;
	float dir = truncate(2*atan2(-posdiff.y,posdiff.x) - fromnode->dir);
	Node* newnode = addnode(pos.x, pos.y, dir);
	addtrack(fromnode, newnode);
	return newnode;

}

void Tracksystem::connecttwonodes(Node* node1, Node* node2)
{
	if(node1==node2)
		return;
	for(auto& track : node1->tracksleft)
		if(track->nodeleft==node2 || track->noderight==node2)
			return;
	for(auto& track : node1->tracksright)
		if(track->nodeleft==node2 || track->noderight==node2)
			return;
	Vec newnodepoint;
	float y1 = -node1->pos.y;
	float y2 = -node2->pos.y;
	float x1 = node1->pos.x;
	float x2 = node2->pos.x;
	float tanth1 = tan(node1->dir);
	float tanth2 = tan(node2->dir);
	float intersectx = (y2-y1+x1*tanth1 - x2*tanth2)/(tanth1 - tanth2);
	float intersecty = -(y1 + (intersectx - x1)*tanth1);
	if(abs(tanth1)>1e5){
		intersectx = x1;
		intersecty = -(y2 + (intersectx - x2)*tanth2);
	}
	Vec tangentintersection(intersectx, intersecty);
	float disttointersect1 = norm(tangentintersection-node1->pos);
	float disttointersect2 = norm(tangentintersection-node2->pos);
	if(disttointersect1 > disttointersect2)
		newnodepoint = tangentintersection + (node1->pos - tangentintersection)/disttointersect1*disttointersect2;
	else
		newnodepoint = tangentintersection + (node2->pos - tangentintersection)/disttointersect2*disttointersect1;
	if(norm(newnodepoint-node1->pos)> 10 && norm(newnodepoint-node2->pos)> 10){
		Node* newnode = extendtrackto(node1, newnodepoint);
		addtrack(newnode, node2);
	}
	else
		addtrack(node1, node2);
}

Node::Node(float xstart, float ystart, float dirstart)
{
	pos.x = xstart;
	pos.y = ystart;
	dir = truncate(dirstart);
	stateleft = 0;
	stateright = 0;
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

Track* Node::getrighttrack()
{
	Track* righttrack = nullptr;
	if(tracksright.size() > stateright)
		righttrack = tracksright[stateright];
	return righttrack;
}

Track* Node::getlefttrack()
{
	Track* lefttrack = nullptr;
	if(tracksleft.size() > stateleft)
		lefttrack = tracksleft[stateleft];
	return lefttrack;
}

void Node::incrementswitch()
{
	stateleft++;
	stateright++;
	if(stateleft>=tracksleft.size())
		stateleft = 0;
	if(stateright>=tracksright.size())
		stateright = 0;
}

Track::Track(Node* left, Node* right)
{
	nodeleft = left;
	noderight = right;

	float dx = cos(nodeleft->dir)*(noderight->pos.x - nodeleft->pos.x) - sin(nodeleft->dir)*(noderight->pos.y - nodeleft->pos.y);
	float dy = sin(nodeleft->dir)*(noderight->pos.x - nodeleft->pos.x) + cos(nodeleft->dir)*(noderight->pos.y - nodeleft->pos.y);
	radius = 0.5*(dy*dy+dx*dx)/dy;
	phi = sign(dy)*atan2(dx, sign(dy)*(radius-dy));
	if(abs(radius)>1e5){
		radius = INFINITY;
		phi = 0;
	}

	if(isrightofleftnode())
		nodeleft->tracksright.push_back(this);
	else
		nodeleft->tracksleft.push_back(this);
	if(isleftofrightnode())
		noderight->tracksleft.push_back(this);
	else
		noderight->tracksright.push_back(this);
	
	std::cout << radius*150*scale/1000 << std::endl;
}

Track::~Track()
{
}

Vec Track::getpos(float nodedist)
{
	return getpos(nodedist, 0);
}

Vec Track::getpos(float nodedist, float transverseoffset)
{
	Vec currentpos;
	Vec leftnodeoffsetpos = nodeleft->pos - Vec(sin(nodeleft->dir), cos(nodeleft->dir))*transverseoffset;
	if(isinf(radius)){
		Vec rightnodeoffsetpos = noderight->pos - Vec(sin(nodeleft->dir), cos(nodeleft->dir))*transverseoffset;
		currentpos = leftnodeoffsetpos + (rightnodeoffsetpos-leftnodeoffsetpos)*nodedist;
	}
	else{
		float ddx, ddy;
		ddx = (radius+transverseoffset)*sin(nodedist*phi);
		ddy = (radius+transverseoffset)*(1-cos(nodedist*phi));
		currentpos = Vec(leftnodeoffsetpos.x + cos(nodeleft->dir)*ddx+sin(nodeleft->dir)*ddy, leftnodeoffsetpos.y - sin(nodeleft->dir)*ddx+cos(nodeleft->dir)*ddy);
	}
	return currentpos;
}

float Track::getarclength(float nodedist)
{
	float arclength;
	if(isinf(radius)){
		float dx = noderight->pos.x - nodeleft->pos.x;
		float dy = -(noderight->pos.y - nodeleft->pos.y);
		arclength = nodedist*sqrt(pow(dx, 2) + pow(dy, 2));
	}
	else{
		arclength = nodedist*abs(radius*phi);
	}
	return arclength;
}

float Track::getorientation(float nodedist)
{
	return nodeleft->dir - nodedist*phi + pi*!isrightofleftnode();
}

bool Track::isrightofleftnode()
{
	if(isinf(radius)){
		if(abs(nodeleft->pos.y - noderight->pos.y)>1)
			return nodeleft->pos.y >= noderight->pos.y;
		else
			if(cos(nodeleft->dir) > 0)
				return (noderight->pos.x >= nodeleft->pos.x);
			else
				return (noderight->pos.x <= nodeleft->pos.x);
	}
	else
		return radius*phi >= 0;
}

bool Track::isleftofrightnode()
{
	return cos(getorientation(1) - noderight->dir) > 0;
}

Track* Track::getrighttrack(){
	Track* righttrack;
	if(isleftofrightnode())
		righttrack = noderight->getrighttrack();
	else
		righttrack = noderight->getlefttrack();
	if(righttrack == nullptr)
		righttrack = this;
	return righttrack;
}

Track* Track::getlefttrack(){
	Track* lefttrack;
	if(isrightofleftnode())
		lefttrack = nodeleft->getlefttrack();
	else
		lefttrack = nodeleft->getrighttrack();
	if(lefttrack == nullptr)
		lefttrack = this;
	return lefttrack;
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
	else SDL_SetRenderDrawColor(renderer, 255*isrightofleftnode(),0, 255*isleftofrightnode(),255);
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

