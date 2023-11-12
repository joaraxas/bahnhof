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

Tracksystem::Tracksystem(std::vector<float> xs, std::vector<float> ys)
{
	for(int iNode = 0; iNode<xs.size(); iNode++)
		nodes.push_back(Node(xs[iNode], ys[iNode], 0));
}

Vec Tracksystem::getpos(Node node1, Node node2, float nodedist)
{
	Vec position = node1.pos + (node2.pos-node1.pos)*nodedist;
	return position;
}

void Tracksystem::render()
{
	for(int iNode=0; iNode<nodes.size()-1; iNode++)
		SDL_RenderDrawLine(renderer, nodes[iNode].pos.x, nodes[iNode].pos.y, nodes[iNode+1].pos.x, nodes[iNode+1].pos.y);
}

Node::Node(float xstart, float ystart, float dirstart)
{
	pos.x = xstart;
	pos.y = ystart;
	dir = dirstart;
}

Train::Train(Tracksystem* newtracksystem)
{
	tracksystem = newtracksystem;
	tex = loadImage("assets/train.png");
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	nodepair[0] = 0;
	nodepair[1] = 1;
	nodedist = 0;
	pos = tracksystem->nodes[nodepair[0]].pos;
}

void Train::update(int ms)
{
	nodedist += ms*0.001*0.2;
	if(nodedist>=1)
	{
		nodepair[0] = nodepair[1];
		nodepair[1] ++;
		nodedist = nodedist-1;
	}
	pos = tracksystem->getpos(tracksystem->nodes[nodepair[0]], tracksystem->nodes[nodepair[1]], nodedist);
	std::cout << pos.x << std::endl;
}

void Train::render()
{
	int x = int(pos.x);
	int y = int(pos.y);
	SDL_Rect srcrect = {0, 0, w, h};
	SDL_Rect rect = {int(x - w / 2), int(y - h / 2), int(w), int(h)};
	SDL_RenderCopyEx(renderer, tex, &srcrect, &rect, -imageangle * 180 / pi, NULL, SDL_FLIP_NONE);
}