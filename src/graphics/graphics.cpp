#pragma once
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
bool allowretina = true;

int init()
{
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
	int windowflags;
	if(allowretina)
		windowflags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
	else
		windowflags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
	window = SDL_CreateWindow("train", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowflags);
	if(window==NULL){
		success = false;
		std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
	if(renderer==NULL){
		success = false;
		std::cout << "Failed to create renderer: " << SDL_GetError() << std::endl;
	}

	SDL_SetRenderDrawColor(renderer, 150, 200, 75, 255);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	res = TTF_Init();
	if(res<0){
		success = false;
		std::cout << "Failed to open SDL_TTF, error code: " << res << ", error: " << TTF_GetError() << std::endl;
	}
	res = setfontsize(12);
	if(res<0)
		success = false;
	return success;
}

int setfontsize(int fontsize)
{
	font = TTF_OpenFont("../assets/fonts/Georgia.ttf", fontsize);
    if(font == NULL)
    {
		std::cout << "Failed to load font, SDL_ttf error: " << TTF_GetError() << std::endl;
        return -1;
    }
	return 1;
}

SDL_Texture* loadimage(std::string path)
{
	SDL_Texture* tex = NULL;
	tex = IMG_LoadTexture(renderer, ("../assets/png/" + path).c_str());
	if(tex==NULL){
		std::cout << "Failed to load texture " << path << ": " << IMG_GetError() << std::endl;
	}
	return tex;	
}

SDL_Texture* loadtext(std::string text, SDL_Color color, int maxwidth)
{
	SDL_Surface* surf = nullptr;
	surf = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), color, maxwidth);
	if(!surf){
		std::cout << "Failed to load surface from text " << text << ", error: " << TTF_GetError() << std::endl;
	}
	SDL_Texture* tex = nullptr;
	tex = SDL_CreateTextureFromSurface(renderer, surf);
	if(!tex){
		std::cout << "Failed to load texture from surface with text " << text << ": " << IMG_GetError() << std::endl;
	}
	else if(SDL_GetRenderTarget(renderer)!=NULL)
		SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_NONE); // This avoids a black outline on text rendered to a transparent surface
    SDL_FreeSurface(surf);
	return tex;	
}

std::pair<int,int> textsize(std::string text, int maxwidth)
{
	SDL_Surface* surf = nullptr;
	surf = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), {0, 0, 0, 0}, maxwidth);
	if(!surf){
		std::cout << "Failed to load surface from text " << text << ", error: " << TTF_GetError() << std::endl;
		return {0,0};	
	}
	int w = surf->w;
	int h = surf->h;
	SDL_FreeSurface(surf);
	return {w, h};
}

void close()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
}

