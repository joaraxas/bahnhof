#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/timing.h"
#include "bahnhof/routing/routing.h"

InterfaceManager::InterfaceManager(Game* newgame)
{
    game = newgame;
    new Panel(this, {SCREEN_WIDTH-300,0,300,SCREEN_HEIGHT});
}

void InterfaceManager::render(Rendering* r)
{
    Gamestate& gamestate = game->getgamestate();

    for(auto& panel: panels)
        panel->render(r);

	if(gamestate.getrouting().selectedroute)
		gamestate.getrouting().selectedroute->render(r);
	else
		gamestate.getrouting().renderroutes(r);

    r->rendertext(std::to_string(int(gamestate.money)) + " Fr", 20, 2*14, {static_cast<Uint8>(127*(gamestate.money<0)),static_cast<Uint8>(63*(gamestate.money>=0)),0,0}, false, false);
	r->rendertext(std::to_string(int(gamestate.time*0.001/60)) + " min", 20, 3*14, {0,0,0,0}, false, false);
	r->rendertext(std::to_string(int(60*float(gamestate.revenue)/float(gamestate.time*0.001/60))) + " Fr/h", 20, 4*14, {0,0,0,0}, false, false);
	r->rendertext(std::to_string(game->gettimemanager().getfps()) + " fps", 20, 5*14, {0,0,0,0}, false, false);
	SDL_SetRenderDrawColor(renderer, 0,0,0,255);
	int scalelinelength = 200;
	r->renderline(Vec(20,SCREEN_HEIGHT-20), Vec(20+scalelinelength,SCREEN_HEIGHT-20), false);
	r->renderline(Vec(20,SCREEN_HEIGHT-20-2), Vec(20,SCREEN_HEIGHT-20+2), false);
	r->renderline(Vec(20+scalelinelength,SCREEN_HEIGHT-20-2), Vec(20+scalelinelength,SCREEN_HEIGHT-20+2), false);
	r->rendertext(std::to_string(int(scalelinelength*0.001*150/r->getscale())) + " m", 20+scalelinelength*0.5-20, SCREEN_HEIGHT-20-14, {0,0,0,0}, false, false);
}

int InterfaceManager::leftclick(Vec mousepos)
{
    for(auto& panel: panels)
        if(panel->click(mousepos, SDL_BUTTON_LEFT))
            return 1;
    return 0;
}

void InterfaceManager::addpanel(Panel* panel)
{
    panels.emplace_back(panel);
}

void InterfaceManager::removepanel(Panel* panel)
{
    auto it = std::find_if(panels.begin(), panels.end(), 
        [panel](const std::unique_ptr<Panel>& ptr){
            return ptr.get() == panel;
        });
    if(it!=panels.end())
        panels.erase(it);
}