#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/track/track.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/ui/ui.h"
#include <localization/localization.h>


void Order::assignroute(Route* newroute, Tracks::Tracksystem* tracksystem)
{
	route = newroute;
	tracks = tracksystem;
}

void Order::invalidate()
{
	if(valid){
		valid = false;
		description = tr("order.invalid", description);
	}
}

Gotostate::Gotostate(State whichstate, bool mustpass)
{
	order = ordertype::gotostate;
	state = whichstate;
	pass = mustpass;
	description = tr("order.go.detailed", state.track, state.nodedist);
}

void Gotostate::assignroute(Route* newroute, Tracks::Tracksystem* tracksystem)
{
	Order::assignroute(newroute, tracksystem);
	tracks->references.trackorders.push_back(this);
	posleft = getpos(*tracks, state, 12);
	posright = getpos(*tracks, state,-12);
}

Gotostate::~Gotostate()
{
	if(tracks)
		tracks->references.removetrackorderreference(this);
}

Setsignal::Setsignal(signalid whichsignal, int redgreenorflip)
{
	order = ordertype::setsignal;
    signal = whichsignal;
    redgreenflip = redgreenorflip;
	if(redgreenflip==0)
		description = tr("order.signal.red", signal);
	else if(redgreenflip==1)
		description = tr("order.signal.green", signal);
	else if(redgreenflip==2)
		description = tr("order.signal.flip", signal);
}

void Setsignal::assignroute(Route* newroute, Tracks::Tracksystem* tracksystem)
{
	offset = 0;
	Order::assignroute(newroute, tracksystem);
	pos = getsignalpos(*tracks, signal);
	tracks->references.signalorders.push_back(this);
	for(int iSignal=0; iSignal<route->signals.size(); iSignal++)
		if(route->signals[iSignal]==signal)
			offset++;
	newroute->signals.push_back(signal);
}

Setsignal::~Setsignal()
{
	if(tracks)
		tracks->references.removesignalorderreference(this);
}

Setswitch::Setswitch(switchid whichswitch, int whichswitchstate)
{
	order = ordertype::setswitch;
    _switch = whichswitch;
	switchstate = whichswitchstate;
	flip = false;
	if(switchstate == -1){
		flip = true;
		description = tr("order.switch.flip", _switch);
	}
	else{
		if(switchstate==0)
			description = tr("order.switch.left", _switch);
		else if(switchstate==1) // TODO: this reads "right" for multitrack switches
			description = tr("order.switch.right", _switch);
		else
			description = tr("order.switch.totrack", _switch, switchstate+1);
	}
}

void Setswitch::assignroute(Route* newroute, Tracks::Tracksystem* tracksystem)
{
	offset = 0;
	Order::assignroute(newroute, tracksystem);
	pos = getswitchpos(*tracks, _switch);
	tracks->references.switchorders.push_back(this);
	for(int iSwitch=0; iSwitch<route->switches.size(); iSwitch++)
		if(route->switches[iSwitch]==_switch)
			offset++;
	newroute->switches.push_back(_switch);
}

Setswitch::~Setswitch()
{
	if(tracks)
		tracks->references.removeswitchorderreference(this);
}

Decouple::Decouple(int keephowmany, Route* givewhatroute)
{
	order = ordertype::decouple;
	where = keephowmany;
	route = givewhatroute;
	if(where==1)
		description = tr("order.decouple.1");
	else
		description = tr("order.decouple.n", where);
	if(route)
		description += tr("order.assignroute", route->name);
}

Turn::Turn()
{
	order = ordertype::turn;
	description = tr("order.reverse");
}

Couple::Couple()
{
	order = ordertype::couple;
	description = tr("order.couple");
}

Loadresource::Loadresource()
{
	order = ordertype::loadresource;
	resource = none;
	anyresource = true;
	loading = true;
	unloading = true;
	description = tr("order.loadunload");
}

Wipe::Wipe()
{
	order = ordertype::wipe;
	description = tr("order.wipe");
}

void Order::renderlabel(Rendering* r, Vec pos, int number, SDL_Color bgrcol, SDL_Color textcol)
{
	int x = int(pos.x); int y = int(pos.y);
	InterfaceManager& ui = tracks->game->getui();
	float scale = ui.getuirendering().getuiscale();
	SDL_Rect rect = {x,y,int(16*scale),int(14*scale)};
	if(!valid)
		bgrcol.a *= 0.4;
	SDL_SetRenderDrawColor(renderer, bgrcol.r, bgrcol.g, bgrcol.b, bgrcol.a);
	r->renderfilledrectangle(rect, true, false);
	r->rendertext(std::to_string(number), x+1*scale, y, textcol);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

void Gotostate::render(Rendering* r, int number)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
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
	float scale = r->getcamscale();
	Vec lineend = pos+Vec(12+18*offset,-7)/scale;
	Vec inlabel = lineend+Vec(0+10,10)/scale;
	Order::renderlabel(r, lineend, number, {0, 0, 0, 255}, {255, 255, 255, 0});
	r->renderline(inlabel + Vec(4-switchstate*4, 0)/scale, inlabel + Vec(switchstate*4, -8)/scale);
}

void Setsignal::render(Rendering* r, int number)
{
	float scale = r->getcamscale();
	Vec lineend = pos+Vec(-8,12+16*offset)/scale;
	SDL_Color bgrcol = {255, 0, 0, 255};
	if(redgreenflip==1)
		bgrcol = {0, 255, 0, 255};
	else if(redgreenflip==2)
		bgrcol = {255, 255, 0, 255};
	Order::renderlabel(r, lineend, number, bgrcol, {0, 0, 0, 255});
}