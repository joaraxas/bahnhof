#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<string>
#include<map>
#include "bahnhof/utils.h"
#include "bahnhof/common/constants.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/track/track.h"

std::vector<std::unique_ptr<Train>> trains;


Train::Train(Tracksystem& newtracksystem, const std::vector<Wagon*> &newwagons, float newspeed)
{
	tracksystem = &newtracksystem;
	wagons = newwagons;
	speed = newspeed;
	for(auto wagon : wagons)
		wagon->train = this;
	lighttex = loadImage("effects/light.png");
	SDL_QueryTexture(lighttex, NULL, NULL, &lightw, &lighth);
	lightw = lightw*0.5;
}

void Train::getinput(int ms)
{
	if(selected){
		if(keys[gasbutton])
			gas(ms);
		if(keys[brakebutton])
			brake(ms);
		if(keys[gearbutton])
			shiftdirection();
		if(keys[routeassignbutton]){}
		else
			for(int iKey=1; iKey<fmin(wagons.size(), sizeof(numberbuttons)/sizeof(*numberbuttons)); iKey++)
				if(keys[numberbuttons[iKey]]) split(iKey);
		if(keys[loadbutton])
			loadall();
		if(keys[unloadbutton])
			unloadall();
		if(keys[couplebutton])
			wantstocouple = true;
	}
}

void Train::update(int ms)
{
	if(go)
		if(perform(ms))
			proceed();
	
	float minradius = INFINITY;
	for(auto w : wagons)
		minradius = fmin(minradius, abs(tracksystem->getradius(w->state)));
	float wagonheight = 2.5;
	float safetyfactor = 0.5;
	float minradiusmeter = minradius*150*0.001;
	float maxspeed = sqrt(safetyfactor*g*minradiusmeter*normalgauge/2/wagonheight)*1000/150;
	
	if(abs(speed)>maxspeed)
		speed = maxspeed*sign(speed);
	float pixels = ms*0.001*abs(speed);
	State newforwardstate = tracksystem->travel(forwardstate(), pixels);
	if(tracksystem->isendofline(newforwardstate)){
		speed = 0;
		pixels = tracksystem->distancefromto(forwardstate(), newforwardstate, pixels);
	}
	tracksystem->runoverblocks(forwardstate(), pixels, this);
	tracksystem->runoverblocks(flipstate(backwardstate()), pixels, nullptr);
	for(auto& wagon : wagons)
		wagon->travel(sign(speed)*pixels);
}

bool Train::perform(int ms)
{
	bool done = false;
	if(route){
	Order* order = route->getorder(orderid);
	if(!order){//orderid does not exist in route
	proceed();
	//if(orderid==-1)//route has no orders
	//brake(ms);
	}
	else{
	switch(order->order){
		case gotostate:{
			Gotostate* specification = dynamic_cast<Gotostate*>(order);
			done = checkifreachedstate(specification->state, ms);
			if(!done){
				if(tracksystem->isred(this))
					brake(ms);
				else
					gas(ms);}
			break;}
		//case 1:
		//	done = checkifleftstate(state); break;
		case o_setsignal:{
			Setsignal* specification = dynamic_cast<Setsignal*>(order);
			tracksystem->setsignal(specification->signal, specification->redgreenflip);
			done = true;
			break;}
		case o_setswitch:{
			Setswitch* specification = dynamic_cast<Setswitch*>(order);
			tracksystem->setswitch(specification->node, specification->updown, specification->nodestate); 
			done = true;
			break;}
		case o_couple:{
			wantstocouple = true;
			done = true;
			break;}
		case decouple:{
			Decouple* specification = dynamic_cast<Decouple*>(order);
			done = split(specification->where, specification->route);
			if(!done)
				brake(ms);
			break;}
		case turn:{
			done = shiftdirection();
			if(!done)
				brake(ms);
			break;}
		case loadresource:{
			Loadresource* specification = dynamic_cast<Loadresource*>(order);
			if(specification->anyresource){
				bool doneloading = true; bool doneunloading = true;
				if(specification->loading)
					doneloading = loadall();
				if(specification->unloading)
					doneunloading = unloadall();
				done = doneloading && doneunloading;
				if(!done)
					brake(ms);
			}
			break;}
		case wipe:{
			route->removeordersupto(orderid);
			done = true;
			break;}
	}
	}
	}
	return done;
}

void Train::proceed()
{
	orderid = route->nextorder(orderid);
}

void Train::render()
{
	if(selected){
		if(route){
			if(orderid>=0){
				int iOrder = route->getindex(orderid);
				int rectw = 10;
				SDL_Rect rect = {SCREEN_WIDTH-300-rectw-2,(iOrder+1)*14+2,rectw,rectw};
				SDL_SetRenderDrawColor(renderer, 200*(!go), 63*go, 0, 255);
				renderfilledrectangle(&rect, false, false);
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			}
		}
		else{
			rendertext("No route assigned", SCREEN_WIDTH-300, (0+1)*14, {0,0,0,0}, false);
		}
		Vec frontpos = tracksystem->getpos(forwardstate());
		float forwarddir = tracksystem->getorientation(forwardstate());
		SDL_Rect forwardrect = {int(frontpos.x-lightw/2), int(frontpos.y-lighth/2), lightw, lighth};
		SDL_Rect srcrect = {0, 0, lightw, lighth};
		rendertexture(lighttex, &forwardrect, &srcrect, forwarddir);
		Vec backpos = tracksystem->getpos(backwardstate());
		float backwarddir = tracksystem->getorientation(backwardstate());
		SDL_Rect backwardrect = {int(backpos.x-lightw/2), int(backpos.y-lighth/2), lightw, lighth};
		srcrect.x = lightw;
		rendertexture(lighttex, &backwardrect, &srcrect, backwarddir);
	}
}

State Train::forwardstate()
{
	if(gasisforward){
		if(wagons.front()->alignedforward)
			return wagons.front()->frontendstate();
		else
			return flipstate(wagons.front()->backendstate());
	}
	else{
		if(wagons.back()->alignedforward)
			return flipstate(wagons.back()->backendstate());
		else
			return wagons.back()->frontendstate();
	}
}

State Train::backwardstate()
{
	if(gasisforward){
		if(wagons.back()->alignedforward)
			return flipstate(wagons.back()->backendstate());
		else
			return wagons.back()->frontendstate();
	}
	else{
		if(wagons.front()->alignedforward)
			return wagons.front()->frontendstate();
		else
			return flipstate(wagons.front()->backendstate());
	}
}

void Train::checkcollision(int ms, Train* train)
{
	if(size(wagons) >= 1)
	if(size(train->wagons) >= 1){
		float pixels = ms*0.001*abs(speed);
		float distance = tracksystem->distancefromto(forwardstate(), flipstate(train->forwardstate()), pixels, true);
		if(distance<pixels){
			for(auto wagon: wagons)
				wagon->travel(distance*sign(speed));
			couple(*train, gasisforward, train->gasisforward);
		}
		else{
			distance = tracksystem->distancefromto(forwardstate(), flipstate(train->backwardstate()), pixels, true);
			if(distance<pixels){
				for(auto wagon: wagons)
					wagon->travel(distance*sign(speed));
				couple(*train, gasisforward, !train->gasisforward);
			}
		}
	}
}

bool Train::checkifreachedstate(State goalstate, int ms)
{
	float pixels = 4*abs(speed)*ms*0.001;
	if(tracksystem->distancefromto(flipstate(forwardstate()), goalstate, pixels)<pixels)
		return true;
	else
		return false;
}

bool Train::gas(int ms)
{
	float Ptot = 0;
	for(auto w : wagons)
		Ptot += w->getpower();
	float mtot = size(wagons);
	speed+=(2*gasisforward-1)*ms*Ptot/mtot;
	if(gasisforward && !wagons.front()->hasdriver)
		speed = fmin(50, speed);
	if(!gasisforward && !wagons.back()->hasdriver)
		speed = -fmin(50, -speed);
	return true;
}

bool Train::brake(int ms)
{
	float Ptot = 0;
	for(auto w : wagons)
		Ptot += 0.2;
	float mtot = size(wagons);
	speed = (2*gasisforward-1)*fmax(0,(2*gasisforward-1)*(speed - (2*gasisforward-1)*ms*Ptot/mtot));
	return (speed==0);
}

bool Train::shiftdirection()
{
	if(speed==0){
		gasisforward = 1-gasisforward;
		speed = 5*(2*gasisforward-1);
		return true;
	}
	else
		return false;
}

bool Train::loadall()
{
	bool done = true;
	if(speed!=0)
		done = false;
	else for(auto w : wagons){
		Storage* storage = getstorageatpoint(w->pos);
		if(storage){
			int unloadedamount = storage->unloadstorage(storage->provides, 1);
			int loadedamount = w->loadwagon(storage->provides, unloadedamount);
			if(loadedamount!=unloadedamount)
				storage->loadstorage(storage->provides, unloadedamount-loadedamount);
			if(loadedamount!=0)
				done = false;
		}
	}
	return done;
}

bool Train::unloadall()
{
	bool done = true;
	if(speed!=0)
		done = false;
	else for(auto w : wagons){
		Storage* storage = getstorageatpoint(w->pos);
		if(storage){
			resourcetype beingunloaded;
			int unloadedamount = w->unloadwagon(&beingunloaded);
			if(storage->accepts==beingunloaded){
				int loadedamount = storage->loadstorage(beingunloaded, unloadedamount);
				if(loadedamount==unloadedamount && loadedamount!=0)
					done = false;
				else
					w->loadwagon(beingunloaded, unloadedamount-loadedamount);
			}
			else
				w->loadwagon(beingunloaded, unloadedamount);
		}
	}
	return done;
}

void Train::couple(Train& train, bool ismyfront, bool ishisfront)
{
	speed = 0;
	train.speed = 0;
	if(wantstocouple || train.wantstocouple){
		bool flipdirection = false;
		bool ismyback = !ismyfront;
		if(ismyback){
			if(ishisfront){
				std::cout << "couple back front" << std::endl;
				wagons.insert(wagons.end(), train.wagons.begin(), train.wagons.end());
			}
			else{
				std::cout << "couple back back" << std::endl;
				wagons.insert(wagons.end(), train.wagons.rbegin(), train.wagons.rend());
				flipdirection = true;
			}
		}
		else{
			if(ishisfront){
				std::cout << "couple front front" << std::endl;
				std::reverse(wagons.begin(), wagons.end());
				wagons.insert(wagons.end(), train.wagons.begin(), train.wagons.end());
				std::reverse(wagons.begin(), wagons.end());
				flipdirection = true;
			}
			else{
				std::cout << "couple front back" << std::endl;
				std::reverse(wagons.begin(), wagons.end());
				wagons.insert(wagons.end(), train.wagons.rbegin(), train.wagons.rend());
				std::reverse(wagons.begin(), wagons.end());
			}
		}
		if(flipdirection)
			for(auto w : train.wagons){
				w->alignedforward = !w->alignedforward;
			}
		train.wagons = {};
		for(auto wagon : wagons)
			wagon->train = this;
		if(train.selected)
			selected = true;
		wantstocouple = false;
	}
	else{
		go = false;
		train.go = false;
	}
}

bool Train::split(int where, Route* assignedroute)
{
	bool splitsucceed = true;
	if(speed!=0)
		splitsucceed = false;
	else if(wagons.size()>where){
		if(gasisforward){
			Train* newtrain = new Train(*tracksystem, {wagons.begin() + where, wagons.end()}, speed);
			wagons = {wagons.begin(), wagons.begin() + where};
			trains.emplace_back(newtrain);
			newtrain->route = assignedroute;
		}
		else{
			Train* newtrain = new Train(*tracksystem, {wagons.rbegin() + where, wagons.rend()}, speed);
			std::reverse(newtrain->wagons.begin(), newtrain->wagons.end());
			wagons = {wagons.rbegin(), wagons.rbegin() + where};
			std::reverse(wagons.begin(), wagons.end());
			trains.emplace_back(newtrain);
			newtrain->route = assignedroute;
		}
		std::cout << "split" << std::endl;
	}
	return splitsucceed;
}
