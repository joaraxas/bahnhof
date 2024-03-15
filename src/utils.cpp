#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<string>
#include<map>
#include "bahnhof/utils.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Rect cam;
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
	if(renderer==NULL){
		success = false;
		std::cout << "Failed to create renderer: " << SDL_GetError() << std::endl;
	}
	SDL_SetRenderDrawColor(renderer, 150, 200, 75, 255);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	cam = {0,0,SCREEN_WIDTH, SCREEN_HEIGHT};
	res = TTF_Init();
	if(res<0){
		success = false;
		std::cout << "Failed to open SDL_TTF, error code: " << res << ", error: " << TTF_GetError() << std::endl;
	}
	font = TTF_OpenFont("assets/fonts/Georgia.ttf", 12);
    if(font == NULL)
    {
		std::cout << "Failed to load font, SDL_ttf error: " << TTF_GetError() << std::endl;
        success = false;
    }
	return success;
}

SDL_Texture* loadImage(std::string path){
	SDL_Texture* tex = NULL;
	tex = IMG_LoadTexture(renderer, ("assets/png/" + path).c_str());
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

void rendertext(std::string text, int x, int y, SDL_Color color, bool ported, bool zoomed){
	SDL_Texture* tex = loadText(text, color);
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	SDL_Rect rect = {x, y, w, h};
	rendertexture(tex, &rect, nullptr, 0, ported, zoomed);
	SDL_DestroyTexture(tex);
}

void rendertexture(SDL_Texture* tex, SDL_Rect* rect, SDL_Rect* srcrect, float angle, bool ported, bool zoomed){
	if(ported){
		rect->x -= cam.x;
		rect->y -= cam.y;
		rect->x *= scale;
		rect->y *= scale;
	}
	if(zoomed){
		rect->w *= scale;
		rect->h *= scale;
	}
	SDL_RenderCopyEx(renderer, tex, srcrect, rect, -angle * 180 / pi, NULL, SDL_FLIP_NONE);
}

void renderline(Vec pos1, Vec pos2, bool ported){
	if(ported){
		pos1.x -= cam.x;
		pos1.y -= cam.y;
		pos2.x -= cam.x;
		pos2.y -= cam.y;
		pos1.x *= scale;
		pos1.y *= scale;
		pos2.x *= scale;
		pos2.y *= scale;
	}
	//if((pos1.x>0 && pos1.x<SCREEN_WIDTH && pos1.y>0 && pos1.y<SCREEN_HEIGHT) || 
	//	(pos2.x>0 && pos2.x<SCREEN_WIDTH && pos2.y>0 && pos2.y<SCREEN_HEIGHT))
		SDL_RenderDrawLine(renderer, pos1.x, pos1.y, pos2.x, pos2.y);
}

void renderrectangle(SDL_Rect* rect, bool ported, bool zoomed){
	if(ported){
		rect->x -= cam.x;
		rect->y -= cam.y;
		rect->x *= scale;
		rect->y *= scale;
	}
	if(zoomed){
		rect->w *= scale;
		rect->h *= scale;
	}
	SDL_RenderDrawRect(renderer, rect);
}

void renderfilledrectangle(SDL_Rect* rect, bool ported, bool zoomed){
	if(ported){
		rect->x -= cam.x;
		rect->y -= cam.y;
		rect->x *= scale;
		rect->y *= scale;
	}
	if(zoomed){
		rect->w *= scale;
		rect->h *= scale;
	}
	SDL_RenderFillRect(renderer, rect);
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

int randint(int maxinclusive)
{
	return rand() % (maxinclusive+1);
}

Vec randpos(int xoffset=0, int yoffset=0)
{
	return Vec(randint(MAP_WIDTH-xoffset), randint(MAP_HEIGHT-yoffset));
};

Gamestate::Gamestate()
{
	money = 600;
	newwagonstate = State(1, 0.2, true);
	initjusttrack();
	inittrain(State(1,0.4,1));
	trains.back()->route = routes.front().get();
	tracksystem->selectednode = 0;

	addtrainstoorphans();
	
	randommap();
}

Gamestate::~Gamestate()
{
	for(int iWagon=0; iWagon<wagons.size(); iWagon++)
		delete wagons[iWagon];
}

void Gamestate::update(int ms)
{
	time += ms;
}

void Gamestate::renderroutes()
{
	int offset = 1;
	for(auto &route : routes){
		rendertext("("+std::to_string(offset)+") "+route->name, SCREEN_WIDTH-300, (offset+1)*14, {0,0,0,0}, false);
		offset++;
	}
	rendertext("(r) Create new route", SCREEN_WIDTH-300, (offset+1)*14, {0,0,0,0}, false);
}

Route* Gamestate::addroute()
{
	Route* newroute = new Route(tracksystem.get(), "Route "+std::to_string(routes.size()+1));
	routes.emplace_back(newroute);
}

void Gamestate::addtrainstoorphans()
{
	for(int iWagon=0; iWagon<wagons.size(); iWagon++){
		if(!wagons[iWagon]->train){
			trains.emplace_back(new Train(*tracksystem, {wagons[iWagon]}, 0));
			std::cout<<"added train automatically"<<std::endl;
		}
	}
}

void Gamestate::randommap()
{
	for(int i=0; i<6; i++){
		Vec newpos = randpos(100,50);
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-randint(storageextraw);
		int storagey = newpos.y-randint(storageextrah);
		storages.emplace_back(new Storage(resources, storagex, storagey, storageextraw+400, storageextrah+400, hops, beer));
		buildings.emplace_back(new Brewery(resources, newpos));
	}
	for(int i=0; i<4; i++){
		Vec newpos = randpos(200,200);
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-randint(storageextraw);
		int storagey = newpos.y-randint(storageextrah);
		storages.emplace_back(new Storage(resources, storagex, storagey, storageextraw+400, storageextrah+400, none, hops));
		buildings.emplace_back(new Hopsfield(resources, newpos));
	}
	for(int i=0; i<6; i++){
		Vec newpos = randpos(100,150);
		int storageextraw = randint(600);
		int storageextrah = randint(600);
		int storagex = newpos.x-randint(storageextraw);
		int storagey = newpos.y-randint(storageextrah);
		storages.emplace_back(new Storage(resources, storagex, storagey, storageextraw+400, storageextrah+400, beer, none));
		buildings.emplace_back(new City(resources, newpos));
	}
}

void Gamestate::initjusttrack()
{
	tracksystem = std::unique_ptr<Tracksystem>(new Tracksystem(resources, {200,700,1000}, {250,250,550}));
	
}

void Gamestate::inittrain(State startstate)
{
	int nWagons = wagons.size();
	wagons.emplace_back(new Locomotive(*tracksystem, startstate));
	for(int iWagon=0; iWagon<3; iWagon++){
		State state = tracksystem->travel(startstate, -(53+49)/2-iWagon*49);
		wagons.emplace_back(new Openwagon(*tracksystem, state));
	}
	trains.emplace_back(new Train(*tracksystem, std::vector<Wagon*>(wagons.begin()+nWagons, wagons.end()), 0));
	
	Route* loadroute = new Route(tracksystem.get(), "Route 1");
	routes.emplace_back(loadroute);
	trains.back()->route = loadroute;
}