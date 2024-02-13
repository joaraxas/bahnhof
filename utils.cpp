#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<string>
#include<map>
#include "utils.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
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
	res = TTF_Init();
	if(res<0){
		success = false;
		std::cout << "Failed to open SDL_TTF, error code: " << res << ", error: " << TTF_GetError() << std::endl;
	}
	font = TTF_OpenFont("assets/Georgia.ttf", 12);
    if(font == NULL)
    {
		std::cout << "Failed to load font, SDL_ttf error: " << TTF_GetError() << std::endl;
        success = false;
    }
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

SDL_Texture* loadText(std::string text, SDL_Color color){
	SDL_Surface* surf = nullptr;
	surf = TTF_RenderUTF8_Solid(font, text.c_str(), color);
	if(!surf){
		std::cout << "Failed to load surface from text " << text << ", error: " << TTF_GetError() << std::endl;
	}
	SDL_Texture* tex = nullptr;
	tex = SDL_CreateTextureFromSurface(renderer, surf);
	if(!tex){
		std::cout << "Failed to load texture from surface with text " << text << ": " << IMG_GetError() << std::endl;
	}
    SDL_FreeSurface(surf);
	return tex;	
}

void rendertext(std::string text, int x, int y, SDL_Color color){
	SDL_Texture* tex = loadText(text, color);
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	SDL_Rect rect = {x, y, w, h};
	SDL_RenderCopy(renderer, tex, NULL, &rect);
}

void close(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
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

float norm(Vec v)
{
	return sqrt(v.x*v.x + v.y*v.y);
}

float sign(float a)
{
	if(a<0)
		return -1;
	return 1;
}

float truncate(float dir)
{
	return dir - pi*floor(dir/pi);
}

Gamestate::Gamestate()
{
	initthreetrains();
	//initcoupling();
	//selectedroute = routes[0].get();

	for(int iWagon=0; iWagon<wagons.size(); iWagon++){
		if(!wagons[iWagon]->train){
			trains.emplace_back(new Train(*tracksystem, {wagons[iWagon]}, 0));
			std::cout<<"added train automatically"<<std::endl;
		}
	}
}

Gamestate::~Gamestate()
{
	for(int iWagon=0; iWagon<wagons.size(); iWagon++)
		delete wagons[iWagon];
}

void Gamestate::initthreetrains()
{
	money = 10;
	tracksystem = std::unique_ptr<Tracksystem>(new Tracksystem(resources, {200,700,800,800,700,200,100,100}, {200,200,300,500,600,600,500,300}));
	tracksystem->leftclick(200, 200);
	tracksystem->rightclick(0, 0);
	tracksystem->leftclick(800, 500);//select
	nodeid firstswitch = tracksystem->selectednode;
	tracksystem->leftclick(800, 600);
	tracksystem->leftclick(700, 700);
	tracksystem->leftclick(200, 700);
	tracksystem->leftclick(100, 500);//connect
	tracksystem->rightclick(0, 0);
	//tracksystem->setswitch(firstswitch, -1);
	signalid upperfields = tracksystem->addsignal(State(5,0.9,true));
	signalid lowerfields = tracksystem->addsignal(State(11,0.9,true));
	signalid enterupper = tracksystem->addsignal(State(5,0.3,true));

	State trainstart;
	int nWagons = 0;
	
	nWagons = wagons.size();
	trainstart = State(2,0.5,true);
	wagons.emplace_back(new Locomotive(*tracksystem, trainstart));
	for(int iWagon=0; iWagon<7; iWagon++){
		State state = tracksystem->travel(trainstart, -(53+49)/2-iWagon*49);
		wagons.emplace_back(new Openwagon(*tracksystem, state));
	}
	trains.emplace_back(new Train(*tracksystem, std::vector<Wagon*>(wagons.begin()+nWagons, wagons.end()), 0));

	nWagons = wagons.size();
	trainstart = State(4,0.9,true);
	wagons.emplace_back(new Locomotive(*tracksystem, trainstart));
	for(int iWagon=0; iWagon<3; iWagon++){
		State state = tracksystem->travel(trainstart, -(53+69)/2-iWagon*69);
		wagons.emplace_back(new Tankwagon(*tracksystem, state));
	}
	trains.emplace_back(new Train(*tracksystem, std::vector<Wagon*>(wagons.begin()+nWagons, wagons.end()), 0));

	nWagons = wagons.size();
	trainstart = State(13,0.5,true);
	wagons.emplace_back(new Locomotive(*tracksystem, trainstart));
	for(int iWagon=0; iWagon<2; iWagon++){
		State state = tracksystem->travel(trainstart, -(53+69)/2-iWagon*69);
		wagons.emplace_back(new Tankwagon(*tracksystem, state));
	}
	trains.emplace_back(new Train(*tracksystem, std::vector<Wagon*>(wagons.begin()+nWagons, wagons.end()), 0));
	
	storages.emplace_back(new Storage(resources, 100,100,400,150, hops, beer));
	storages.emplace_back(new Storage(resources, 300,500,600,250, beer, hops));
	buildings.emplace_back(new Brewery(resources, 150,120,100,50));
	buildings.emplace_back(new Hopsfield(resources, 625,625,50,50));
	buildings.emplace_back(new City(resources, 700,625,20,50));

	routes.emplace_back(new Route("Hops"));
	int ri = size(routes)-1;
	routes[ri]->appendorder(new Loadresource());
	routes[ri]->appendorder(new Gotostate(State(3,0.7,true)));
	routes[ri]->appendorder(new Setswitch(firstswitch, 00, 1));
	routes[ri]->appendorder(new Gotostate(State(11,0.7,true)));
	routes[ri]->appendorder(new Loadresource());
	routes[ri]->appendorder(new Setsignal(upperfields, 0));
	routes[ri]->appendorder(new Gotostate(State(1,0.5,true)));
	routes[ri]->appendorder(new Setsignal(upperfields, 1));
	trains[0]->route = routes[ri].get();

	routes.emplace_back(new Route("Beer"));
	ri = size(routes)-1;
	routes[ri]->appendorder(new Gotostate(State(3,0.7,true)));
	routes[ri]->appendorder(new Setswitch(firstswitch, 00, 0));
	routes[ri]->appendorder(new Gotostate(State(5,0.7,true)));
	routes[ri]->appendorder(new Setsignal(enterupper, 0));
	routes[ri]->appendorder(new Loadresource());
	routes[ri]->appendorder(new Setsignal(lowerfields, 0));
	routes[ri]->appendorder(new Gotostate(State(6,0.5,true)));
	routes[ri]->appendorder(new Setsignal(enterupper, 1));
	routes[ri]->appendorder(new Gotostate(State(1,0.5,true)));
	routes[ri]->appendorder(new Setsignal(lowerfields, 1));
	routes[ri]->appendorder(new Loadresource());
	trains[1]->route = routes[ri].get();
	trains[2]->route = routes[ri].get();
}

void Gamestate::initcoupling()
{
	tracksystem = std::unique_ptr<Tracksystem>(new Tracksystem(resources, {200,700,800,800,700,200,100,100}, {200,200,300,500,600,600,500,300}));
	tracksystem->leftclick(200, 200);
	tracksystem->rightclick(0, 0);
	tracksystem->leftclick(800, 500);//select
	nodeid rightswitch = tracksystem->selectednode;
	tracksystem->leftclick(800, 600);
	tracksystem->leftclick(700, 700);
	tracksystem->leftclick(200, 700);
	tracksystem->leftclick(100, 500);//connect
	nodeid leftswitch = tracksystem->selectednode;
	tracksystem->rightclick(0, 0);
	tracksystem->setswitch(rightswitch, 1);
	tracksystem->setswitch(leftswitch, 0);
	State topstation(1,0.6,true);
	State upperstation(5,0.8,true);
	State lowerstation(11,0.8,true);
	State turnpoint(7,0.4,true);

	State trainstart;
	int nWagons = 0;
	
	nWagons = wagons.size();
	trainstart = topstation;
	wagons.emplace_back(new Locomotive(*tracksystem, trainstart));
	for(int iWagon=0; iWagon<3; iWagon++){
		State state = tracksystem->travel(trainstart, -(53+49)/2-iWagon*49);
		wagons.emplace_back(new Openwagon(*tracksystem, state));
	}
	trains.emplace_back(new Train(*tracksystem, std::vector<Wagon*>(wagons.begin()+nWagons, wagons.end()), 0));
	
	nWagons = wagons.size();
	trainstart = upperstation;
	for(int iWagon=0; iWagon<3; iWagon++){
		State state = tracksystem->travel(trainstart, -iWagon*69);
		wagons.emplace_back(new Tankwagon(*tracksystem, state));
	}
	trains.emplace_back(new Train(*tracksystem, std::vector<Wagon*>(wagons.begin()+nWagons, wagons.end()), 0));

	storages.emplace_back(new Storage(resources, 100,100,400,150, hops, beer));
	storages.emplace_back(new Storage(resources, 300,500,600,250, beer, hops));
	buildings.emplace_back(new Brewery(resources, 150,120,100,50));
	buildings.emplace_back(new Hopsfield(resources, 625,625,50,50));
	buildings.emplace_back(new City(resources, 700,625,20,50));

	int ri = 0;

	ri = size(routes);
	Route* loadroute = new Route("Load up");
	routes.emplace_back(loadroute);
	routes[ri]->appendorder(new Loadresource());

	ri = size(routes);
	routes.emplace_back(new Route("Go get"));
	routes[ri]->appendorder(new Setswitch(rightswitch, 00, 1));
	routes[ri]->appendorder(new Gotostate(lowerstation));
	routes[ri]->appendorder(new Decouple(loadroute));
	routes[ri]->appendorder(new Gotostate(turnpoint));
	routes[ri]->appendorder(new Setswitch(leftswitch, 00, 0));
	routes[ri]->appendorder(new Turn());
	routes[ri]->appendorder(new Gotostate(upperstation));
	routes[ri]->appendorder(new Turn());
	routes[ri]->appendorder(new Gotostate(topstation));
	routes[ri]->appendorder(new Loadresource());
	routes[ri]->appendorder(new Setswitch(rightswitch, 00, 0));
	routes[ri]->appendorder(new Gotostate(upperstation));
	routes[ri]->appendorder(new Decouple(loadroute));
	routes[ri]->appendorder(new Gotostate(turnpoint));
	routes[ri]->appendorder(new Setswitch(leftswitch, 00, 1));
	routes[ri]->appendorder(new Turn());
	routes[ri]->appendorder(new Gotostate(lowerstation));
	routes[ri]->appendorder(new Turn());
	routes[ri]->appendorder(new Gotostate(topstation));
	routes[ri]->appendorder(new Loadresource());
	trains[0]->route = routes[ri].get();
}