#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/ui/buttons.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"


InterfaceManager::InterfaceManager(Game* newgame)
{
    game = newgame;
    new UI::MainPanel(this, {0,0,200*getlogicalscale(),200*getlogicalscale()});
}

void InterfaceManager::update(int ms)
{
    if(dropdown){
        dropdown->update(ms);
    }
    for(auto& panel: panels)
        panel->update(ms);
}

void InterfaceManager::render(Rendering* r)
{
    int scale = getlogicalscale();
    int viewheight = r->getviewsize().y;
    renderscalemeasurer(r, scale*20, viewheight-scale*20, scale*200);

    for(auto& panel: panels)
        panel->render(r);

    if(dropdown)
        dropdown->render(r);
}

void InterfaceManager::renderscalemeasurer(Rendering* r, int leftx, int lefty, int scalelinelength)
{
    int scale = getlogicalscale();

    int textheight = 14*scale;
    SDL_SetRenderDrawColor(renderer, 0,0,0,255);
    int viewheight = r->getviewsize().y;
    int offset = leftx;
    int markersize = 2*scale;
	r->renderline(Vec(leftx,lefty), Vec(leftx+scalelinelength,lefty), false);
	r->renderline(Vec(leftx,lefty-markersize), Vec(leftx,lefty+markersize), false);
	r->renderline(Vec(leftx+scalelinelength,lefty-markersize), Vec(leftx+scalelinelength,lefty+markersize), false);
	r->rendertext(std::to_string(int(scalelinelength*0.001*150/r->getcamscale())) + " m", leftx+scalelinelength*0.5-leftx, lefty-textheight, {0,0,0,0}, false, false);

}

int InterfaceManager::leftclick(Vec mousepos)
{
    bool clicked = false;
    if(dropdown){
        if(dropdown->checkclick(mousepos, SDL_BUTTON_LEFT)){
            clicked = true;
        }
        setdropdown(nullptr);
    }
    if(!clicked)
        for(auto& panel: panels)
            if(panel->click(mousepos, SDL_BUTTON_LEFT))
                return true;
    return clicked;
}

void InterfaceManager::addpanel(UI::Host* panel)
{
    panels.emplace_back(panel);
}

void InterfaceManager::removepanel(UI::Host* panel)
{
    auto it = std::find_if(panels.begin(), panels.end(), 
        [panel](const std::unique_ptr<UI::Host>& ptr){
            return ptr.get() == panel;
        });
    if(it!=panels.end())
        panels.erase(it);
}

void InterfaceManager::setdropdown(UI::Dropdown* dr)
{
    if(dropdown)
        delete dropdown;
    dropdown = dr;
}

Game& InterfaceManager::getgame()
{
    return *game;
}

int InterfaceManager::getlogicalscale()
{
    return game->getrendering().getlogicalscale();
}

namespace UI{
Element::Element(Panel* newpanel)
{
    panel = newpanel;
    ui = &panel->getui();
    game = &ui->getgame();
}

bool Element::checkclick(Vec mousepos, int type)
{
    SDL_Rect absrect = getglobalrect();
	if(mousepos.x>=absrect.x && mousepos.x<=absrect.x+absrect.w && mousepos.y>=absrect.y && mousepos.y<=absrect.y+absrect.h){
		return true;
	}
    return false;
}

SDL_Rect Element::getglobalrect()
{
    Vec panelpos = panel->topcorner();
    return {rect.x+int(panelpos.x), rect.y+int(panelpos.y), rect.w, rect.h};
}

SDL_Rect Element::getlocalrect()
{
    return rect;
}
}