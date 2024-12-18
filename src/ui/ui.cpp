#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/buttons.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/timing.h"


InterfaceManager::InterfaceManager(Game* newgame)
{
    game = newgame;
    new UI::MainPanel(this, {0,0,200*getlogicalscale(),200*getlogicalscale()});
}

void InterfaceManager::render(Rendering* r)
{
    Gamestate& gamestate = game->getgamestate();

    for(auto& panel: panels)
        panel->render(r);
    
    int scale = getlogicalscale();
    int textheight = 14*scale;
    r->rendertext(std::to_string(int(gamestate.money)) + " Fr", 220, 2*textheight, {static_cast<Uint8>(127*(gamestate.money<0)),static_cast<Uint8>(63*(gamestate.money>=0)),0,0}, false, false);
	r->rendertext(std::to_string(int(gamestate.time*0.001/60)) + " min", 220, 3*textheight, {0,0,0,0}, false, false);
	r->rendertext(std::to_string(int(60*float(gamestate.revenue)/float(gamestate.time*0.001/60))) + " Fr/h", 220, 4*textheight, {0,0,0,0}, false, false);
	r->rendertext(std::to_string(game->gettimemanager().getfps()) + " fps", 220, 5*textheight, {0,0,0,0}, false, false);
	SDL_SetRenderDrawColor(renderer, 0,0,0,255);
	int scalelinelength = scale*200;
    int viewheight = r->getviewsize().y;
    int offset = 20*scale;
    int markersize = 2*scale;
	r->renderline(Vec(offset,viewheight-offset), Vec(offset+scalelinelength,viewheight-offset), false);
	r->renderline(Vec(offset,viewheight-offset-markersize), Vec(offset,viewheight-offset+markersize), false);
	r->renderline(Vec(offset+scalelinelength,viewheight-offset-markersize), Vec(offset+scalelinelength,viewheight-offset+markersize), false);
	r->rendertext(std::to_string(int(scalelinelength*0.001*150/r->getcamscale())) + " m", offset+scalelinelength*0.5-offset, viewheight-offset-textheight, {0,0,0,0}, false, false);
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

int InterfaceManager::getlogicalscale()
{
    return game->getrendering().getlogicalscale();
}