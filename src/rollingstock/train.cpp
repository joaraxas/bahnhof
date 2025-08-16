#include<iostream>
#include<string>
#include<map>
#include<algorithm>
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/input/input.h"
#include "bahnhof/track/track.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/resources/storage.h"
#include "bahnhof/ui/panels.h"


Train::Train(Tracks::Tracksystem& newtracksystem, const std::vector<Wagon*> &newwagons)
{
	tracksystem = &newtracksystem;
	game = tracksystem->game;
	wagons = newwagons;
	panel = std::make_unique<UI::Owner>();
	speed = 0;
	for(auto wagon : wagons)
		wagon->train = this;
	light.setspritesheet(game->getsprites(), sprites::light);
}

Train::~Train()
{}

void Train::getinput(InputManager* input, int ms)
{
	if(selected){
		if(input->iskeypressed(gasbutton))
			gas(ms);
		if(input->iskeypressed(brakebutton))
			brake(ms);
		if(input->iskeypressed(gearbutton))
			shiftdirection();
		if(input->iskeypressed(loadbutton))
			loadall();
		if(input->iskeypressed(unloadbutton))
			unloadall();
		if(input->iskeypressed(couplebutton))
			wantstocouple = true;
	}
}

void Train::update(int ms)
{
	if(go)
		if(perform(ms))
			proceed();
	
	float minradius = INFINITY;
	// uncomment to remove radius speed restriction
	// /*
	for(auto w : wagons)
		minradius = fmin(minradius, abs(getradius(*tracksystem, w->axes->frontendstate())));
	// */
	float wagonheight = 2.5;
	float safetyfactor = 0.5;
	float minradiusmeter = minradius*150*0.001;
	float maxspeed = sqrt(safetyfactor*g*minradiusmeter*normalgauge/2/wagonheight)*1000/150;
	
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

bool Train::perform(int ms)
{
	bool done = false;
	if(route){
	Order* order = route->getorder(orderid);
	if(!order){//orderid does not exist in route
		proceed();
	}
	else if(!order->valid){
		proceed();
	}
	else{
	switch(order->order){
		case gotostate:{
			Gotostate* specification = dynamic_cast<Gotostate*>(order);
			done = checkifreachedstate(specification->state, ms);
			if(!done){
				if(Tracks::Signaling::isred(*tracksystem, this))
					brake(ms);
				else
					gas(ms);}
			break;}
		//case 1:
		//	done = checkifleftstate(state); break;
		case o_setsignal:{
			Setsignal* specification = dynamic_cast<Setsignal*>(order);
			setsignal(*tracksystem, specification->signal, specification->redgreenflip);
			done = true;
			break;}
		case o_setswitch:{
			Setswitch* specification = dynamic_cast<Setswitch*>(order);
			setswitch(*tracksystem, specification->_switch, specification->switchstate); 
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

void Train::render(Rendering* r)
{
	if(selected){
		Vec frontpos = getpos(*tracksystem, forwardstate());
		float forwarddir = getorientation(*tracksystem, forwardstate());
		light.imagetype = 0;
		light.imageangle = forwarddir;
		light.render(r, frontpos);
		Vec backpos = getpos(*tracksystem, backwardstate());
		float backwarddir = getorientation(*tracksystem, backwardstate());
		light.imagetype = 1;
		light.imageangle = backwarddir;
		light.render(r, backpos);
	}
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
	if(size(wagons) >= 1)
	if(size(train->wagons) >= 1){
		float pixels = ms*0.001*abs(speed);
		float distance = Tracks::distancefromto(*tracksystem, forwardstate(), flipstate(train->forwardstate()), pixels, true);
		if(distance<pixels){
			for(auto wagon: wagons)
				wagon->axes->travel(distance*sign(speed));
			couple(*train, gasisforward, train->gasisforward);
		}
		else{
			distance = Tracks::distancefromto(*tracksystem, forwardstate(), flipstate(train->backwardstate()), pixels, true);
			if(distance<pixels){
				for(auto wagon: wagons)
					wagon->axes->travel(distance*sign(speed));
				couple(*train, gasisforward, !train->gasisforward);
			}
		}
	}
}

bool Train::checkifreachedstate(State goalstate, int ms)
{
	float pixels = 4*abs(speed)*ms*0.001;
	if(Tracks::distancefromto(*tracksystem, flipstate(forwardstate()), goalstate, pixels)<pixels)
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
	// uncomment to remove train backwards speed restriction
	// /*
	if(gasisforward && !wagons.front()->hasdriver)
		speed = fmin(50, speed);
	if(!gasisforward && !wagons.back()->hasdriver)
		speed = -fmin(50, -speed);
	// */
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
	if(speed!=0 || where<1 || wagons.size()<=1)
		return false;
	where = std::fmin(where, wagons.size()-1);
	TrainManager& trainmanager = game->getgamestate().gettrainmanager();
	if(gasisforward){
		Train* newtrain = new Train(*tracksystem, {wagons.begin() + where, wagons.end()});
		wagons = {wagons.begin(), wagons.begin() + where};
		trainmanager.addtrain(newtrain);
		newtrain->route = assignedroute;
	}
	else{
		Train* newtrain = new Train(*tracksystem, {wagons.rbegin() + where, wagons.rend()});
		std::reverse(newtrain->wagons.begin(), newtrain->wagons.end());
		wagons = {wagons.rbegin(), wagons.rbegin() + where};
		std::reverse(wagons.begin(), wagons.end());
		trainmanager.addtrain(newtrain);
		newtrain->route = assignedroute;
	}
	std::cout << "split" << std::endl;
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

void Train::select()
{
	selected = true;
	if(!panel->exists()){
		panel->set(new UI::TrainPanel(&game->getui(), 
									  {300,200,400,220}, 
									  game->getgamestate().gettrainmanager(), 
									  *this));
	}
}

void Train::deselect()
{
	selected = false;
}