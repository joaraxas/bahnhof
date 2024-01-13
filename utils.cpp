#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<string>
#include "utils.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
const Uint8* keys = NULL;

int init(){
	bool success = true;
	std::srand((unsigned) time(NULL));
	int sdlflags = SDL_INIT_VIDEO;
	int res = SDL_Init(sdlflags);
	if(res<0){
		success = false;
		std::cout << "Failed to open SDL2, error code: " << res << ", error: " << SDL_GetError() << std::endl;
	}
	int imgflags = IMG_INIT_PNG;
	res = IMG_Init(imgflags);
	if(!(res & imgflags)){
		success = false;
		std::cout << "Failed to open SDL_Image, error code: " << res << ", error: " << IMG_GetError() << std::endl;
	}
	window = SDL_CreateWindow("train", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if(window==NULL){
		success = false;
		std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(window==NULL){
		success = false;
		std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
	}
	SDL_SetRenderDrawColor(renderer, 150, 200, 75, 255);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	return success;
}

SDL_Texture* loadImage(std::string path){
	SDL_Texture* tex = NULL;
	tex = IMG_LoadTexture(renderer, path.c_str());
	if(tex==NULL){
		std::cout << "Failed to load texture " << path << ": " << IMG_GetError() << std::endl;
	}
	return tex;	
}

void close(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
}

Vec::Vec()
{
	x = 0;
	y = 0;
}

Vec::Vec(float xstart, float ystart)
{
	x = xstart;
	y = ystart;
}

Vec Vec::operator-(Vec v)
{
	Vec out(0,0);
	out.x = x-v.x;
	out.y = y-v.y;
	return out;
}

Vec Vec::operator+(Vec v)
{
	Vec out(0,0);
	out.x = x+v.x;
	out.y = y+v.y;
	return out;
}

Vec Vec::operator*(float a)
{
	Vec out(0,0);
	out.x = x*a;
	out.y = y*a;
	return out;
}

Vec Vec::operator/(float a)
{
	Vec out(0,0);
	out.x = x/a;
	out.y = y/a;
	return out;
}

float norm(Vec v){
	return sqrt(v.x*v.x + v.y*v.y);
}

float sign(float a){
	if(a<0)
		return -1;
	return 1;
}

float truncate(float dir){
	return dir - pi*floor(dir/pi);
}

Tracksystem::Tracksystem(std::vector<float> xs, std::vector<float> ys)
{
	nodes.push_back(std::unique_ptr<Node>{new Node(xs[0], ys[0], 0)});
	for(int iNode = 1; iNode<xs.size(); iNode++){
		addnode(xs[iNode], ys[iNode], nodes[iNode-1].get());
		addtrack(nodes[iNode-1].get(), nodes[iNode].get(), iNode);
	}
	selectednode = nodes.back().get();
}

void Tracksystem::addnode(float x, float y, Node* previousnode){
	float dx = (x - previousnode->pos.x);
	float dy = -(y - previousnode->pos.y);
	float dir = truncate(2*atan2(dy,dx) - previousnode->dir);
	nodes.push_back(std::unique_ptr<Node>{new Node(x, y, dir)});
}

void Tracksystem::addtrack(Node* leftnode, Node* rightnode, int ind){
	tracks.push_back(std::unique_ptr<Track>{new Track(leftnode, rightnode, ind)});
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
		addtrack(selectednode, nodes.back().get(), nodes.size()-1);
		if(mindistsquared>pow(20,2)){}
		else{
			addtrack(nodes.back().get(), nearestnode, nodes.size()-1);
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

Track::Track(Node* left, Node* right, int ind)
{
	nodeleft = left;
	noderight = right;
	radius = getradius();
	float dx = cos(nodeleft->dir)*(noderight->pos.x - nodeleft->pos.x) - sin(nodeleft->dir)*(noderight->pos.y - nodeleft->pos.y);
	float dy = sin(nodeleft->dir)*(noderight->pos.x - nodeleft->pos.x) + cos(nodeleft->dir)*(noderight->pos.y - nodeleft->pos.y);
	y0 = 0.5*(dy*dy+dx*dx)/dy;
	phi = sign(dy)*atan2(dx, sign(dy)*(y0-dy));
	indexx = ind;
	if(y0*phi>=0 || radius==INFINITY)
		nodeleft->tracksright.push_back(this);
	else
		nodeleft->tracksleft.push_back(this);
	if(cos(-(sign(y0*phi)-1)/2*pi+nodeleft->dir-phi-noderight->dir)>0) //TODO: y0*phi is nan when straight
		noderight->tracksleft.push_back(this);
	else
		noderight->tracksright.push_back(this);
}

Track::~Track()
{
	//std::cout<<radius<<std::endl;
}

float Track::getradius()
{
	float dx = noderight->pos.x - nodeleft->pos.x;
	float dy = -(noderight->pos.y - nodeleft->pos.y);
	float distance = sqrt(pow(dx, 2) + pow(dy, 2));
	float anglebetween = atan2(dy, dx);
	if(abs(sin(anglebetween-nodeleft->dir))<0.01)
		return INFINITY;
	else
		return distance/abs(2*sin(anglebetween-nodeleft->dir));
}

Vec Track::getpos(float nodedist)
{
	Vec currentpos;
	if(isinf(radius)){
		currentpos = nodeleft->pos + (noderight->pos-nodeleft->pos)*nodedist;
	}
	else{
		float ddx, ddy;
		ddx = y0*sin(nodedist*phi);
		ddy = y0*(1-cos(nodedist*phi));
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
		arclength = abs(y0*phi);
	}
	return arclength;
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

Node::Node(float xstart, float ystart, float dirstart)
{
	pos.x = xstart;
	pos.y = ystart;
	dir = truncate(dirstart);
	stateleft = 0;
	stateright = 0;
}

Train::Train(Tracksystem* newtracksystem, float nodediststart)
{
	tracksystem = newtracksystem;
	tex = loadImage("assets/train.png");
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	h = h/2;
	track = tracksystem->tracks[0].get();
	nodedist = nodediststart;
	pos = track->getpos(nodedist);
	speed = 0.;
}

void Train::getinput()
{
	//if(selected){
		if(keys[gasbutton]) speed+=1;
		if(keys[breakbutton]) speed-=1;
	//}
}

void Train::update(int ms)
{
	if(connectedleft!=nullptr){
		float delta = norm(pos-connectedleft->pos) - 35;
		std::cout << norm(pos-connectedleft->pos) << std::endl;
		float dv = speed - connectedleft->speed;
		//connectedleft->speed += 0.06*delta*ms + 0.01*ms*dv;
	}
	if(connectedright!=nullptr){
		float delta = norm(pos-connectedright->pos) - 35;
		float dv = speed - connectedright->speed;
		//connectedright->speed += -0.06*delta*ms + 0.01*ms*dv;
	}

	float arclength1 = track->getarclength(1);
	nodedist += ms*0.001*speed*(alignedwithtrackdirection*2-1)/arclength1;
	if(nodedist>=1)
	{
		Node* currentnode = track->noderight;
		if(cos(-(sign(track->y0*track->phi)-1)/2*pi+track->nodeleft->dir-track->phi-track->noderight->dir)>0){ //TODO: y0*phi is nan when straight
			track = currentnode->tracksright[currentnode->stateright];
		}
		else{
			track = currentnode->tracksleft[currentnode->stateleft];
		}
		float arclength2 = track->getarclength(1);
		if(track->nodeleft==currentnode)
			nodedist = (nodedist-1)*arclength1/arclength2;
		else{
			nodedist = 1-(nodedist-1)*arclength1/arclength2;
			alignedwithtrackdirection = 1-alignedwithtrackdirection;
		}
	}
	else if(nodedist<0)
	{
		Node* currentnode = track->nodeleft;
		if(track->y0*track->phi>=0 || track->radius==INFINITY){
			track = currentnode->tracksleft[currentnode->stateleft];
		}
		else{
			track = currentnode->tracksright[currentnode->stateright];
		}
		float arclength2 = track->getarclength(1);
		if(track->nodeleft==currentnode){
			nodedist = (-nodedist)*arclength1/arclength2;
			alignedwithtrackdirection = 1-alignedwithtrackdirection;
		}
		else{
			nodedist = 1-(-nodedist)*arclength1/arclength2;
		}
	}

	pos = track->getpos(nodedist);
	imageangle = track->nodeleft->dir - sign(track->radius)*nodedist*(track->phi);
}

void Train::render()
{
	int x = int(pos.x);
	int y = int(pos.y);
	SDL_Rect srcrect = {0, 0, w, h};
	SDL_Rect rect = {int(x - w / 2), int(y - h / 2), int(w), int(h)};
	SDL_RenderCopyEx(renderer, tex, &srcrect, &rect, -imageangle * 180 / pi, NULL, SDL_FLIP_NONE);
}
