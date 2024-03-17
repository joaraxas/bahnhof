#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/rendering.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/track/track.h"
#include "bahnhof/resources/resources.h"


void Order::assignroute(Route* newroute)
{
	route = newroute;
}

Gotostate::Gotostate(State whichstate, bool mustpass)
{
	order = gotostate;
	state = whichstate;
	pass = mustpass;
	description = "Reach state at track " + std::to_string(state.track) + " and nodedist " + std::to_string(state.nodedist);
}

Setsignal::Setsignal(signalid whichsignal, int redgreenorflip)
{
	order = o_setsignal;
    signal = whichsignal;
    redgreenflip = redgreenorflip;
	if(redgreenflip==0)
		description = "Set signal " + std::to_string(signal) + " to red";
	else if(redgreenflip==1)
		description = "Set signal " + std::to_string(signal) + " to green";
	else if(redgreenflip==2)
		description = "Flip signal " + std::to_string(signal);
}

void Setsignal::assignroute(Route* newroute)
{
	offset = 0;
	Order::assignroute(newroute);
	for(int iSignal=0; iSignal<route->signals.size(); iSignal++)
		if(route->signals[iSignal]==signal)
			offset++;
	newroute->signals.push_back(signal);
}

Setswitch::Setswitch(nodeid whichnode, bool upordown, int whichnodestate)
{
	order = o_setswitch;
    node = whichnode;
    updown = upordown;
	nodestate = whichnodestate;
	flip = false;
	std::string switchname = "switch " + std::to_string(node);
	if(updown) switchname+= " up"; else switchname+= " down";
	if(nodestate == -1){
		flip = true;
		description = "Flip " + switchname;
	}
	else{
		if(nodestate==0)
			description = "Set " + switchname + " to left";
		else if(nodestate==1)
			description = "Set " + switchname + " to right";
		else
			description = "Set " + switchname + " to track no. " + std::to_string(nodestate+1) + " counting from left";
	}
}

void Setswitch::assignroute(Route* newroute)
{
	offset = 0;
	Order::assignroute(newroute);
	for(int iSwitch=0; iSwitch<route->switches.size(); iSwitch++)
		if(route->switches[iSwitch]==node && route->updowns[iSwitch]==updown)
			offset++;
	newroute->switches.push_back(node);
	newroute->updowns.push_back(updown);
}

Decouple::Decouple(int keephowmany, Route* givewhatroute)
{
	order = decouple;
	where = keephowmany;
	route = givewhatroute;
	description = "Decouple";
	if(where>1)
		description += " " + std::to_string(where) + " wagons";
	if(route)
		description += ", assign route " + route->name + " to other half";
}

Turn::Turn()
{
	order = turn;
	description = "Switch travel direction";
}

Couple::Couple()
{
	order = o_couple;
	description = "Prepare for coupling";
}

Loadresource::Loadresource()
{
	order = loadresource;
	resource = none;
	anyresource = true;
	loading = true;
	unloading = true;
	description = "Load and unload all possible cargo";
}

Wipe::Wipe()
{
	order = wipe;
	description = "Wipe all previous orders";
}

void Order::renderlabel(Rendering* r, Vec pos, int number, SDL_Color bgrcol, SDL_Color textcol)
{
	int x = int(pos.x); int y = int(pos.y);
	SDL_Rect rect = {x,y,16,14};
	SDL_SetRenderDrawColor(renderer, bgrcol.r, bgrcol.g, bgrcol.b, bgrcol.a);
	r->renderfilledrectangle(&rect, true, false);
	r->rendertext(std::to_string(number), x+1, y, textcol);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

void Gotostate::render(Rendering* r, int number)
{
	Vec posleft = route->tracksystem->getpos(state, 12);
	Vec posright = route->tracksystem->getpos(state,-12);
	r->renderline(posleft, posright);
	if(posright.x>=posleft.x){
		Order::renderlabel(r, posright, number);
	}
	else{
		Order::renderlabel(r, posleft, number);
	}
}

void Setswitch::render(Rendering* r, int number)
{
	float scale = r->getscale();
	Vec pos = route->tracksystem->getswitchpos(node, updown);
	Vec lineend = pos+Vec(12+18*offset,-7)/scale;
	Vec inlabel = lineend+Vec(0+10,10)/scale;
	Order::renderlabel(r, lineend, number, {0, 0, 0, 255}, {255, 255, 255, 0});
	r->renderline(inlabel + Vec(4-nodestate*4, 0)/scale, inlabel + Vec(nodestate*4, -8)/scale);
}

void Setsignal::render(Rendering* r, int number)
{
	float scale = r->getscale();
	Vec pos = route->tracksystem->getsignalpos(signal);
	Vec lineend = pos+Vec(-8,12+16*offset)/scale;
	SDL_Color bgrcol = {255, 0, 0, 255};
	if(redgreenflip==1)
		bgrcol = {0, 255, 0, 255};
	else if(redgreenflip==2)
		bgrcol = {255, 255, 0, 255};
	Order::renderlabel(r, lineend, number, bgrcol, {0, 0, 0, 255});
}
