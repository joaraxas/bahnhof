#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/buttons.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/timing.h"


InterfaceManager::InterfaceManager(Game* newgame)
{
    game = newgame;
    new UI::MainPanel(this, {0,0,200,200});
}

void InterfaceManager::render(Rendering* r)
{
    Gamestate& gamestate = game->getgamestate();

    for(auto& panel: panels)
        panel->render(r);

    r->rendertext(std::to_string(int(gamestate.money)) + " Fr", 220, 2*14, {static_cast<Uint8>(127*(gamestate.money<0)),static_cast<Uint8>(63*(gamestate.money>=0)),0,0}, false, false);
	r->rendertext(std::to_string(int(gamestate.time*0.001/60)) + " min", 220, 3*14, {0,0,0,0}, false, false);
	r->rendertext(std::to_string(int(60*float(gamestate.revenue)/float(gamestate.time*0.001/60))) + " Fr/h", 220, 4*14, {0,0,0,0}, false, false);
	r->rendertext(std::to_string(game->gettimemanager().getfps()) + " fps", 220, 5*14, {0,0,0,0}, false, false);
	SDL_SetRenderDrawColor(renderer, 0,0,0,255);
	int scalelinelength = 200;
    int viewheight = r->getviewsize().y;
	r->renderline(Vec(20,viewheight-20), Vec(20+scalelinelength,viewheight-20), false);
	r->renderline(Vec(20,viewheight-20-2), Vec(20,viewheight-20+2), false);
	r->renderline(Vec(20+scalelinelength,viewheight-20-2), Vec(20+scalelinelength,viewheight-20+2), false);
	r->rendertext(std::to_string(int(scalelinelength*0.001*150/r->getscale())) + " m", 20+scalelinelength*0.5-20, viewheight-20-14, {0,0,0,0}, false, false);
}

int InterfaceManager::leftclick(Vec mousepos)
{
    for(auto& panel: panels)
        if(panel->click(mousepos, SDL_BUTTON_LEFT))
            return 1;
    return 0;
}

void InterfaceManager::addpanel(UI::Panel* panel)
{
    panels.emplace_back(panel);
}

void InterfaceManager::removepanel(UI::Panel* panel)
{
    auto it = std::find_if(panels.begin(), panels.end(), 
        [panel](const std::unique_ptr<UI::Panel>& ptr){
            return ptr.get() == panel;
        });
    if(it!=panels.end())
        panels.erase(it);
}

Game& InterfaceManager::getgame()
{
    return *game;
}