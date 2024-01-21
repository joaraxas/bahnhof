#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<string>
#include<map>
#include "utils.h"

Tracksystem::Tracksystem(std::vector<float> xs, std::vector<float> ys)
{
	nodes.push_back(std::unique_ptr<Node>{new Node(xs[0], ys[0], 0)});
	for(int iNode = 1; iNode<xs.size(); iNode++){
		addnode(xs[iNode], ys[iNode], nodes[iNode-1].get());
		addtrack(nodes[iNode-1].get(), nodes[iNode].get());
	}
	selectednode = nodes.back().get();
}

void Tracksystem::addnode(float x, float y, Node* previousnode){
	float dx = (x - previousnode->pos.x);
	float dy = -(y - previousnode->pos.y);
	float dir = truncate(2*atan2(dy,dx) - previousnode->dir);
	nodes.push_back(std::unique_ptr<Node>{new Node(x, y, dir)});
}

void Tracksystem::addtrack(Node* leftnode, Node* rightnode){
	tracks.push_back(std::unique_ptr<Track>{new Track(leftnode, rightnode)});
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
	float mindistsquared = INFINITY;
	Node* nearestnode;
	for(auto& node: nodes){
		float distsquared = pow(node->pos.x-xMouse, 2) + pow(node->pos.y-yMouse, 2);
		if(distsquared < mindistsquared){
			nearestnode = node.get();
			mindistsquared = distsquared;
		}
	}
	if(selectednode!=nullptr){
		Vec newnodepoint;
		if(mindistsquared>pow(20,2)){
			newnodepoint.x = xMouse;
			newnodepoint.y = yMouse;
		}
		else{
			float y1 = -selectednode->pos.y;
			float y2 = -nearestnode->pos.y;
			float x1 = selectednode->pos.x;
			float x2 = nearestnode->pos.x;
			float tanth1 = tan(selectednode->dir);
			float tanth2 = tan(nearestnode->dir);
			float intersectx = (y2-y1+x1*tanth1 - x2*tanth2)/(tanth1 - tanth2);
			float intersecty = -(y1 + (intersectx - x1)*tanth1);
			if(abs(tanth1)>1e5){
				intersectx = x1;
				intersecty = -(y2 + (intersectx - x2)*tanth2);
			}
			Vec tangentintersection(intersectx, intersecty);
			float disttointersect1 = norm(tangentintersection-selectednode->pos);
			float disttointersect2 = norm(tangentintersection-nearestnode->pos);
			if(disttointersect1 > disttointersect2)
				newnodepoint = tangentintersection + (selectednode->pos - tangentintersection)/disttointersect1*disttointersect2;
			else
				newnodepoint = tangentintersection + (nearestnode->pos - tangentintersection)/disttointersect2*disttointersect1;
		}
		addnode(newnodepoint.x, newnodepoint.y, selectednode);
		addtrack(selectednode, nodes.back().get());
		if(mindistsquared>pow(20,2)){}
		else{
			addtrack(nodes.back().get(), nearestnode);
		}
		selectednode = nodes.back().get();
	}
	else{
		selectednode = nearestnode;
	}

}

void Tracksystem::rightclick(int xMouse, int yMouse)
{
	selectednode = nullptr;

	float mindistsquared = INFINITY;
	Node* nearestnode;
	for(auto& node: nodes){
		float distsquared = pow(node->pos.x-xMouse, 2) + pow(node->pos.y-yMouse, 2);
		if(distsquared < mindistsquared){
			nearestnode = node.get();
			mindistsquared = distsquared;
		}
	}
	if(mindistsquared<=pow(20,2)){
		nearestnode->incrementswitch();
	}
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
	if(isinf(radius))
		return nodeleft->pos.y >= noderight->pos.y;
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
		float sleeperwidth = 2600/200;
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
	if(nicetracks) gauge = 1435/200;
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

