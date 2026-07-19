#include "bahnhof/routing/routefollower.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/track/track.h"
#include "bahnhof/rollingstock/train.h"

void RouteFollower::setorderid(int id)
{
	if(!route || !route->getorder(id))
		return;
	orderid = id;
}

int RouteFollower::getorderid()
{
	return orderid;
}

bool RouteFollower::perform(int ms)
{
	if(!route)
		return false;
	Order* order = route->getorder(orderid);
	if(!order || !order->valid){
		proceed();
		return false;
	}
	bool done = false;
	switch(order->order){
		case ordertype::gotostate:{
			Gotostate* specification = dynamic_cast<Gotostate*>(order);
			done = train.checkifreachedstate(specification->state, ms);
			if(!done){
				if(Tracks::Signaling::isred(*tracksystem, &train))
					train.brake(ms);
				else
					train.gas(ms);}
			break;
		}
		//case 1:
		//	done = checkifleftstate(state); break;
		case ordertype::setsignal:{
			Setsignal* specification = dynamic_cast<Setsignal*>(order);
			setsignal(*tracksystem, specification->signal, specification->redgreenflip);
			done = true;
			break;
		}
		case ordertype::setswitch:{
			Setswitch* specification = dynamic_cast<Setswitch*>(order);
			setswitch(*tracksystem, specification->_switch, specification->switchstate); 
			done = true;
			break;
		}
		case ordertype::couple:{
			train.wantstocouple = true;
			done = true;
			break;
		}
		case ordertype::decouple:{
			Decouple* specification = dynamic_cast<Decouple*>(order);
			done = train.split(specification->where, specification->route);
			if(!done)
				train.brake(ms);
			break;
		}
		case ordertype::turn:{
			done = train.shiftdirection();
			if(!done)
				train.brake(ms);
			break;
		}
		case ordertype::loadresource:{
			Loadresource* specification = dynamic_cast<Loadresource*>(order);
			if(specification->anyresource){
				bool doneloading = true; bool doneunloading = true;
				if(specification->loading)
					doneloading = train.loadall();
				if(specification->unloading)
					doneunloading = train.unloadall();
				done = doneloading && doneunloading;
				if(!done)
					train.brake(ms);
			}
			break;
		}
		case ordertype::wipe:{
			route->removeordersupto(orderid);
			done = true;
			break;
		}
	}
	if(done)
		proceed();
	return done;
}

void RouteFollower::proceed()
{
	if(!route)
		return;
	orderid = route->nextorder(orderid);
}