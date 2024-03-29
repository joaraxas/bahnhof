#pragma once
#include<iostream>
#include<string>
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;

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

SDL_Texture* loadimage(std::string path)
{
	SDL_Texture* tex = NULL;
	tex = IMG_LoadTexture(renderer, ("../assets/png/" + path).c_str());
	if(tex==NULL){
		std::cout << "Failed to load texture " << path << ": " << IMG_GetError() << std::endl;
	}
	return tex;	
}

SDL_Texture* loadtext(std::string text, SDL_Color color)
{
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

