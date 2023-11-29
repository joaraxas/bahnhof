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

float norm(Vec v){
	return sqrt(v.x*v.x + v.y*v.y);
}

float sign(float a){
	if(a<0)
		return -1;
	return 1;
}

Tracksystem::Tracksystem(std::vector<float> xs, std::vector<float> ys)
{
	nodes.push_back(std::unique_ptr<Node>{new Node(xs[0], ys[0], 0)});
	for(int iNode = 1; iNode<xs.size(); iNode++){
		addnode(xs[iNode], ys[iNode], nodes[iNode-1].get(), 1);
		addtrack(nodes[iNode-1].get(), nodes[iNode].get(), iNode);
	}
	selectednode = nodes.back().get();
}

void Tracksystem::addnode(float x, float y, Node* previousnode, int leftright){
	float dx = (x - previousnode->pos.x);
	float dy = -(y - previousnode->pos.y);
	float dir = (2*atan2(dy,dx) - previousnode->dir);
	if(dir <= -pi)
		dir += 2*pi;
	if(dir >= pi)
		dir -= 2*pi;
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
		float nodex, nodey;
		int leftright = 1;
		Vec dv = Vec(xMouse, yMouse) - selectednode->pos;
		if(cos(selectednode->dir)*dv.x - sin(selectednode->dir)*dv.y < 0)
			leftright = -1;
		if(mindistsquared>pow(20,2)){
			nodex = xMouse;
			nodey = yMouse;
		}
		else{
			float dx = nearestnode->pos.x - selectednode->pos.x;
			float dy = -(nearestnode->pos.y - selectednode->pos.y);
			float dxintersect = (dy-dx*tan(nearestnode->dir))/(tan(selectednode->dir)-tan(nearestnode->dir));
			Vec tangentintersection = selectednode->pos + Vec(dxintersect, -tan(selectednode->dir)*dxintersect);
			float db = leftright*norm(nearestnode->pos - tangentintersection);
			Vec newnodepoint = tangentintersection - Vec(db*cos(selectednode->dir), -db*sin(selectednode->dir));
			nodex = newnodepoint.x;
			nodey = newnodepoint.y;
		}
		addnode(nodex, nodey, selectednode, leftright);
		if(leftright==1)
			addtrack(selectednode, nodes.back().get(), nodes.size()-1);
		else
			addtrack(nodes.back().get(), selectednode, nodes.size()-1);
		if(mindistsquared>pow(20,2)){}
		else{
			if(leftright==1)
				addtrack(nodes.back().get(), nearestnode, nodes.size()-1);
			else
				addtrack(nearestnode, nodes.back().get(), nodes.size()-1);
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
}

Track::Track(Node* left, Node* right, int ind)
{
	nodeleft = left;
	noderight = right;
	radius = getradius();
	indexx = ind;
	std::cout<<"radius: "<<radius<<std::endl;
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
	std::cout<<"angle between: "<<anglebetween-nodeleft->dir<<std::endl;
	if(abs(sin(anglebetween-nodeleft->dir))<0.01)
		return INFINITY;
	else
		return distance/(2*sin(anglebetween-nodeleft->dir));
}

Vec Track::getpos(float nodedist)
{
	//radius = getradius();
	float ddx, ddy;
	if(isinf(radius)){
		ddx = getarclength(nodedist);
		ddy = 0;
	}
	else{
		float phi;
		if(sign(fmod(noderight->dir - nodeleft->dir+20*pi+pi,2*pi)-pi)*sign(radius)>0)
			phi = nodedist*(fmod(fmod(noderight->dir - nodeleft->dir+pi,2*pi)-pi-pi,2*pi)+pi);
		else{
			phi = nodedist*(fmod(fmod((noderight->dir-pi) - (nodeleft->dir)+pi,2*pi)-pi-pi,2*pi)+pi);
			std::cout<<"active"<<std::endl;
		}
		//if(sign(radius)*sign(phi) < 0)
		//	phi = nodedist*(fmod(fmod(nodeleft->dir - noderight->dir+pi,2*pi)-pi-pi,2*pi)+pi);
		ddx = radius*sin(phi);
		ddy = -radius*(1-cos(phi));
	}
	return Vec(nodeleft->pos.x + cos(nodeleft->dir)*ddx+sin(nodeleft->dir)*ddy, nodeleft->pos.y - sin(nodeleft->dir)*ddx+cos(nodeleft->dir)*ddy);
}

float Track::getarclength(float nodedist)
{
	float arclength;
	//radius = getradius();
	if(isinf(radius)){
		float dx = noderight->pos.x - nodeleft->pos.x;
		float dy = -(noderight->pos.y - nodeleft->pos.y);
		arclength = nodedist*sqrt(pow(dx, 2) + pow(dy, 2));
	}
	else{
		float phi = nodedist*(noderight->dir - nodeleft->dir);
		arclength = radius*phi;
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
	dir = dirstart;
	std::cout<<"dir: "<<dir<<std::endl;
}
/*
Train::Train(Tracksystem* newtracksystem)
{
	tracksystem = newtracksystem;
	tex = loadImage("assets/train.png");
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	h = h/2;
	nodepair[0] = 0;
	nodepair[1] = 1;
	nodedist = 0.;
	pos = tracksystem->nodes[nodepair[0]].pos;
	speed = 50.;
}

void Train::update(int ms)
{
	float arclength1 = tracksystem->getarclength(tracksystem->nodes[nodepair[0]], tracksystem->nodes[nodepair[1]], 1);
	nodedist += ms*0.001*speed/arclength1;
	if(nodedist>=1)
	{
		nodepair[0] = nodepair[1];
		nodepair[1] ++;
		float arclength2 = tracksystem->getarclength(tracksystem->nodes[nodepair[0]], tracksystem->nodes[nodepair[1]], 1);
		nodedist = (nodedist-1)*arclength1/arclength2;
	}
	pos = tracksystem->getpos(tracksystem->nodes[nodepair[0]], tracksystem->nodes[nodepair[1]], nodedist);
	imageangle = (1-nodedist)*(tracksystem->nodes[nodepair[0]].dir) + nodedist*(tracksystem->nodes[nodepair[1]].dir);
}

void Train::render()
{
	int x = int(pos.x);
	int y = int(pos.y);
	SDL_Rect srcrect = {0, 0, w, h};
	SDL_Rect rect = {int(x - w / 2), int(y - h / 2), int(w), int(h)};
	SDL_RenderCopyEx(renderer, tex, &srcrect, &rect, -imageangle * 180 / pi, NULL, SDL_FLIP_NONE);
}
*/