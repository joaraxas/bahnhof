#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/rendering.h"

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
	font = TTF_OpenFont("../assets/fonts/Georgia.ttf", 12);
    if(font == NULL)
    {
		std::cout << "Failed to load font, SDL_ttf error: " << TTF_GetError() << std::endl;
        success = false;
    }
	return success;
}

SDL_Texture* loadImage(std::string path){
	SDL_Texture* tex = NULL;
	tex = IMG_LoadTexture(renderer, ("../assets/png/" + path).c_str());
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

Vec randpos(int xoffset, int yoffset)
{
	return Vec(randint(MAP_WIDTH-xoffset), randint(MAP_HEIGHT-yoffset));
};
