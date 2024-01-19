#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
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
		SDL_RenderDrawLine(renderer, node->pos.x-5, node->pos.y-5, node->pos.x+5, node->pos.y+5);
		SDL_RenderDrawLine(renderer, node->pos.x-5, node->pos.y+5, node->pos.x+5, node->pos.y-5);
		SDL_SetRenderDrawColor(renderer, 255*(node->dir>2*pi), 0, 255*(node->dir<-2*pi), 255);
		SDL_RenderDrawLine(renderer, node->pos.x, node->pos.y, node->pos.x+12*cos(node->dir), node->pos.y-12*sin(node->dir));
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
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
		nearestnode->stateleft++;
		nearestnode->stateright++;
		if(nearestnode->stateleft>=nearestnode->tracksleft.size())
			nearestnode->stateleft = 0;
		if(nearestnode->stateright>=nearestnode->tracksright.size())
			nearestnode->stateright = 0;
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

Track::Track(Node* left, Node* right)
{
	nodeleft = left;
	noderight = right;
	float dx = cos(nodeleft->dir)*(noderight->pos.x - nodeleft->pos.x) - sin(nodeleft->dir)*(noderight->pos.y - nodeleft->pos.y);
	float dy = sin(nodeleft->dir)*(noderight->pos.x - nodeleft->pos.x) + cos(nodeleft->dir)*(noderight->pos.y - nodeleft->pos.y);
	radius = 0.5*(dy*dy+dx*dx)/dy;
	phi = sign(dy)*atan2(dx, sign(dy)*(radius-dy));
	//if(isrightofleftnode() || radius==INFINITY)
	if(isrightofleftnode())
		nodeleft->tracksright.push_back(this);
	else
		nodeleft->tracksleft.push_back(this);
	//if(cos(-(sign(radius*phi)-1)/2*pi+nodeleft->dir-phi-noderight->dir)>0) //TODO: radius*phi is nan when straight
	if(cos(getorientation(1)-noderight->dir)>0){
		noderight->tracksleft.push_back(this);
		std::cout << "more" <<std::endl;
		std::cout<< getorientation(1) << std::endl;
		std::cout<< noderight->dir << std::endl;
	}
	else{
		noderight->tracksright.push_back(this);
		std::cout << "less" <<std::endl;
		std::cout<< getorientation(1) << std::endl;
		std::cout<< noderight->dir << std::endl;
	}
}

Track::~Track()
{
}

Vec Track::getpos(float nodedist)
{
	Vec currentpos;
	if(isinf(radius)){
		currentpos = nodeleft->pos + (noderight->pos-nodeleft->pos)*nodedist;
	}
	else{
		float ddx, ddy;
		ddx = radius*sin(nodedist*phi);
		ddy = radius*(1-cos(nodedist*phi));
		currentpos = Vec(nodeleft->pos.x + cos(nodeleft->dir)*ddx+sin(nodeleft->dir)*ddy, nodeleft->pos.y - sin(nodeleft->dir)*ddx+cos(nodeleft->dir)*ddy);
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
		arclength = abs(radius*phi);
	}
	return arclength;
}

float Track::getorientation(float nodedist)
{
	return nodeleft->dir - nodedist*phi + pi*!isrightofleftnode();
}

bool Track::isrightofleftnode()
{
	return (radius*phi)>=0;
}

void Track::render()
{
	int nSegments = 50;
	for(float nodedist = 0; nodedist < 1; nodedist+=1./nSegments){
		Vec drawpos1 = getpos(nodedist);
		Vec drawpos2 = getpos(nodedist+1./nSegments);
		if(drawpos2.x>0)
		if(drawpos2.x<SCREEN_WIDTH)
		if(drawpos2.y>0)
		if(drawpos2.y<SCREEN_HEIGHT)
		SDL_RenderDrawLine(renderer, drawpos1.x, drawpos1.y, drawpos2.x, drawpos2.y);
	}
}

