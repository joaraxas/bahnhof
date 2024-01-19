#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<string>
#include "utils.h"

Resource* selectedresource = NULL;
std::vector<std::unique_ptr<Train>> trains;


Train::Train(Tracksystem* newtracksystem, const std::vector<Wagon*> &newwagons, float newspeed)
{
	tracksystem = newtracksystem;
	wagons = newwagons;
	speed = newspeed;
	for(auto wagon : wagons)
		wagon->train = this;
}

void Train::getinput(int ms)
{
	if(selected){
		if(keys[gasbutton]){
			float Ptot = 0;
			for(auto w : wagons)
				if(abs(speed)<w->maxspeed[(w->alignedforward)==direction])
					Ptot += w->P[(w->alignedforward)==direction];
			float mtot = size(wagons);
			speed+=(2*direction-1)*ms*Ptot/mtot;
		}
		if(keys[breakbutton]){
			float Ptot = 0;
			for(auto w : wagons)
				Ptot += 0.2;
			float mtot = size(wagons);
			speed = (2*direction-1)*fmax(0,(2*direction-1)*(speed - (2*direction-1)*ms*Ptot/mtot));
		}
		if(keys[gearbutton]){
			if(speed==0){
				direction = 1-direction;
				speed = 5*(2*direction-1);
			}
		}
		if(keys[loadbutton]){
			for(auto w : wagons)
				w->loadwagon(*selectedresource, 1);
		}
		if(keys[unloadbutton]){
			for(auto w : wagons){
				int unloadedamount = w->unloadwagon();
				money += unloadedamount;
			}
			std::cout << money << std::endl;
		}
		for(int iKey=1; iKey<fmin(wagons.size(), sizeof(numberbuttons)/sizeof(*numberbuttons)); iKey++)
			if(keys[numberbuttons[iKey]]) split(iKey);
	}
}

void Train::checkCollision(Train* train)
{
	if(size(wagons) >= 1)
	if(size(train->wagons) >= 1)
	if(abs(speed - train->speed)>10){
		if(norm(wagons.back()->pos - train->wagons.front()->pos) <= 35)
			couple(*train, true, false);
		else if(norm(wagons.back()->pos - train->wagons.back()->pos) <= 35)
			couple(*train, true, true);
		else if(norm(wagons.front()->pos - train->wagons.front()->pos) <= 35)
			couple(*train, false, false);
		else if(norm(wagons.front()->pos - train->wagons.back()->pos) <= 35)
			couple(*train, false, true);
	}
}

void Train::couple(Train& train, bool ismyback, bool ishisback)
{
	bool flipdirection = false;
	if(ismyback && !ishisback){
		std::cout << "couple back front" << std::endl;
		flipdirection = wagons.back()->alignedwithtrackdirection != train.wagons.front()->alignedwithtrackdirection;
		wagons.insert(wagons.end(), train.wagons.begin(), train.wagons.end());
	}
	else if(ismyback && ishisback){
		std::cout << "couple back back" << std::endl;
		flipdirection = wagons.back()->alignedwithtrackdirection != train.wagons.back()->alignedwithtrackdirection;
		wagons.insert(wagons.end(), train.wagons.rbegin(), train.wagons.rend());
	}
	else if(!ismyback && !ishisback){
		std::cout << "couple front front" << std::endl;
		flipdirection = wagons.front()->alignedwithtrackdirection != train.wagons.front()->alignedwithtrackdirection;
		std::reverse(wagons.begin(), wagons.end());
		wagons.insert(wagons.end(), train.wagons.begin(), train.wagons.end());
	}
	else if(!ismyback && ishisback){
		std::cout << "couple front back" << std::endl;
		flipdirection = wagons.front()->alignedwithtrackdirection != train.wagons.back()->alignedwithtrackdirection;
		std::reverse(wagons.begin(), wagons.end());
		wagons.insert(wagons.end(), train.wagons.rbegin(), train.wagons.rend());
	}
	if(flipdirection)
		for(auto w : train.wagons){
			w->alignedwithtrackdirection = 1 - w->alignedwithtrackdirection;
			w->alignedforward = 1 - w->alignedforward;
		}
	train.wagons = {};
	for(auto wagon : wagons)
		wagon->train = this;
}

void Train::split(int where)
{
	if(wagons.size()>where){
		trains.emplace_back(new Train(tracksystem, {wagons.begin() + where, wagons.end()}, speed));
		wagons = {wagons.begin(), wagons.begin() + where};
		std::cout << "split" << std::endl;
	}
}

Resource::Resource(resourcetype newtype, std::string newname, std::string pathtotex)
{
    type = newtype;
	name = newname;
	tex = loadImage(pathtotex);
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
}

void Resource::render(Vec pos)
{
	int x = int(pos.x);
	int y = int(pos.y);
	SDL_Rect srcrect = {0, 0, w, h};
	SDL_Rect rect = {int(x - w / 2), int(y - h / 2), int(w), int(h)};
	SDL_RenderCopyEx(renderer, tex, &srcrect, &rect, -0 * 180 / pi, NULL, SDL_FLIP_NONE);
}

