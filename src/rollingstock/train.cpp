#include<map>
#include<algorithm>
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/input/input.h"
#include "bahnhof/track/track.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/resources/storage.h"
#include "bahnhof/ui/panels.h"


Train::Train(Tracks::Tracksystem& newtracksystem, const std::vector<Wagon*> &newwagons) :
	routefollower(*this, newtracksystem)
{
	tracksystem = &newtracksystem;
	game = tracksystem->game;
	wagons = newwagons;
	speed = 0;
	for(auto wagon : wagons)
		wagon->train = this;
	light.setspritesheet(game->getsprites(), sprites::light);
}

void Train::update(int ms)
{
	routefollower.trigger(ms);
	
	float minradius = INFINITY;
	// comment to remove radius speed restriction
	for(auto w : wagons)
		minradius = fmin(minradius, abs(getradius(*tracksystem, w->axes->frontendstate())));
	float wagonheight = 2.5;
	float safetyfactor = 0.5;
	float minradiusmeter = pixelstometers(minradius);
	float maxspeed_m_s = sqrt(safetyfactor*g*minradiusmeter*normalgauge/2/wagonheight);
	float maxspeed = meterstopixels(maxspeed_m_s);

	if(abs(speed)>maxspeed)
		speed = maxspeed*sign(speed);
	float pixels = ms*0.001*abs(speed);
	State newforwardstate = Tracks::travel(*tracksystem, forwardstate(), pixels);
	if(Tracks::isendofline(*tracksystem, newforwardstate)){
		speed = 0;
		pixels = Tracks::distancefromto(*tracksystem, forwardstate(), newforwardstate, pixels);
	}
	Tracks::Signaling::runoverblocks(*tracksystem, forwardstate(), pixels, this);
	Tracks::Signaling::runoverblocks(*tracksystem, flipstate(backwardstate()), pixels, nullptr);
	for(auto& wagon : wagons)
		wagon->axes->travel(sign(speed)*pixels);
}

void Train::render(Rendering* r)
{
	Vec frontpos = getpos(*tracksystem, forwardstate());
	Angle forwarddir = getorientation(*tracksystem, forwardstate());
	light.imagetype = 0;
	light.imageangle = forwarddir;
	light.render(r, frontpos);
	Vec backpos = getpos(*tracksystem, backwardstate());
	Angle backwarddir = getorientation(*tracksystem, backwardstate());
	light.imagetype = 1;
	light.imageangle = backwarddir;
	light.render(r, backpos);
}

State Train::forwardstate()
{
	if(gasisforward){
		if(wagons.front()->alignedforward)
			return wagons.front()->axes->frontendstate();
		else
			return flipstate(wagons.front()->axes->backendstate());
	}
	else{
		if(wagons.back()->alignedforward)
			return flipstate(wagons.back()->axes->backendstate());
		else
			return wagons.back()->axes->frontendstate();
	}
}

State Train::backwardstate()
{
	if(gasisforward){
		if(wagons.back()->alignedforward)
			return flipstate(wagons.back()->axes->backendstate());
		else
			return wagons.back()->axes->frontendstate();
	}
	else{
		if(wagons.front()->alignedforward)
			return wagons.front()->axes->frontendstate();
		else
			return flipstate(wagons.front()->axes->backendstate());
	}
}

void Train::checkcollision(int ms, Train* train)
{
	if(speed==0 || wagons.empty() || train->wagons.empty())
		return;
	float pixels = ms*0.001*abs(speed);
	float distance = Tracks::distancefromto(*tracksystem, forwardstate(), 
		flipstate(train->forwardstate()), pixels, true);
	if(distance<pixels){
		distance-=0.0001; // prevents comparison of equal floats
		for(auto wagon: wagons)
			wagon->axes->travel(distance*sign(speed));
		couple(*train, gasisforward, train->gasisforward);
		return;
	}
	distance = Tracks::distancefromto(*tracksystem, forwardstate(), 
		flipstate(train->backwardstate()), pixels, true);
	if(distance<pixels){
		distance-=0.0001; // prevents comparison of equal floats
		for(auto wagon: wagons)
			wagon->axes->travel(distance*sign(speed));
		couple(*train, gasisforward, !train->gasisforward);
	}
}

bool Train::checkifreachedstate(State goalstate, int ms)
{
	float pixels = 4*abs(speed)*ms*0.001;
	if(Tracks::distancefromto(
		*tracksystem, flipstate(forwardstate()), goalstate, pixels) < pixels
	)
		return true;
	else
		return false;
}

bool Train::cangas()
{
	for(auto w : wagons)
		if(w->getpower() > 0)
			return true;
	return false;
}

void Train::gas(int ms)
{
	float Ptot = 0;
	for(auto w : wagons)
		Ptot += w->getpower();
	float mtot = size(wagons);
	speed+=(2*gasisforward-1)*ms*Ptot/mtot;
	// uncomment to remove train backwards speed restriction
	// /*
	// if(gasisforward && !wagons.front()->hasdriver)
	// 	speed = fmin(50, speed);
	// if(!gasisforward && !wagons.back()->hasdriver)
	// 	speed = -fmin(50, -speed);
	// */
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
			resourcetype provided = storage->getfirstprovidedresource();
			int unloadedamount = storage->unloadstorage(provided, 1);
			int loadedamount = w->loadwagon(provided, unloadedamount);
			if(loadedamount!=unloadedamount)
				storage->loadstorage(provided, unloadedamount-loadedamount);
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
			int unloadedamount = w->unloadwagon(beingunloaded);
			if(storage->accepts(beingunloaded)){
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
				wagons.insert(wagons.end(), train.wagons.begin(), train.wagons.end());
			}
			else{
				wagons.insert(wagons.end(), train.wagons.rbegin(), train.wagons.rend());
				flipdirection = true;
			}
		}
		else{
			if(ishisfront){
				std::reverse(wagons.begin(), wagons.end());
				wagons.insert(wagons.end(), train.wagons.begin(), train.wagons.end());
				std::reverse(wagons.begin(), wagons.end());
				flipdirection = true;
			}
			else{
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
		std::cout<<"couple me: "<<name<<std::endl;
		std::cout<<"to other train: "<<train.name<<std::endl;
		if(train.panel.exists()){
			createpanel();
		}
		wantstocouple = false;
		train.wantstocouple = false;
	}
	else{
		routefollower.go = false;
		train.routefollower.go = false;
	}
}

bool Train::split(int where, Route* assignedroute)
{
	if(speed!=0 || where<1 || wagons.size()<=1)
		return false;
	where = std::fmin(where, wagons.size()-1);
	TrainManager& trainmanager = game->getgamestate().gettrainmanager();
	if(gasisforward){
		Train* newtrain = new Train(*tracksystem, {wagons.begin() + where, wagons.end()});
		wagons = {wagons.begin(), wagons.begin() + where};
		trainmanager.addtrain(newtrain);
		newtrain->routefollower.route = assignedroute;
	}
	else{
		Train* newtrain = new Train(*tracksystem, {wagons.rbegin() + where, wagons.rend()});
		std::reverse(newtrain->wagons.begin(), newtrain->wagons.end());
		wagons = {wagons.rbegin(), wagons.rbegin() + where};
		std::reverse(wagons.begin(), wagons.end());
		trainmanager.addtrain(newtrain);
		newtrain->routefollower.route = assignedroute;
	}
	return true;
}

TrainInfo Train::getinfo()
{
	std::vector<WagonInfo> wagoninfos;
	for(auto& wagon : wagons){
		wagoninfos.push_back(wagon->getinfo());
	}
	return TrainInfo(this, name, speed, wagoninfos);
}

void Train::createpanel()
{
	if(!panel.exists()){
		panel.set(new UI::TrainPanel(&game->getui(), 
									  game->getgamestate().gettrainmanager(), 
									  *this,
									  routefollower));
	}
}
