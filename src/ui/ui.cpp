#include<iostream>
#include "bahnhof/common/input.h"
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/ui/buttons.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/common/gamestate.h"


InterfaceManager::InterfaceManager(Game* newgame)
{
    game = newgame;
    new UI::MainPanel(this, {0,0,200,300});
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
    float scale = getlogicalscale();
    int viewheight = r->getviewsize().y;
    renderscaleruler(r, scale*20, viewheight-scale*20, 200*scale);

    for(auto pit = panels.rbegin(); pit!=panels.rend(); ++pit)
        (*pit)->render(r);

    if(dropdown)
        dropdown->render(r);
}

SDL_Rect InterfaceManager::rendertext(Rendering* r, std::string text, SDL_Rect rect, UI::TextStyle style, bool centered)
{
    SDL_Color color;
    switch (style)
    {
    case UI::Info:
        color = {0,0,0,255};
        break;
    case UI::Highlighted:
        color = {255,255,255,255};
        break;
    case UI::InvertedInfo:
        color = {255,255,255,255};
        break;
    default:
        std::cout<<"Warning: UI style "<<style<<" not supported in InterfaceManager::rendertext"<<std::endl;
        break;
    }
    float scale = getlogicalscale();
    SDL_Rect absrect = uitoscreen(rect);
    SDL_Rect textrect;
    if(centered)
        textrect = r->rendercenteredtext(text, absrect.x+absrect.w*0.5, absrect.y+absrect.h*0.5, color, false, false, absrect.w);
    else
        textrect = r->rendertext(text, absrect.x, absrect.y, color, false, false, absrect.w);
    return screentoui(textrect);
}

void InterfaceManager::renderscaleruler(Rendering* r, int leftx, int lefty, int scalelinelength)
{
    float scale = getlogicalscale();

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

UI::Host* InterfaceManager::getpanelat(Vec screenpos)
{
    for(auto& panel: panels){
        if(panel->checkclick(screenpos))
            return panel.get();
    }
    return nullptr;
}

bool InterfaceManager::mousehover(Vec mousepos, int ms)
{
    if(dropdown){
        if(dropdown->checkclick(mousepos)){
            dropdown->mousehover(mousepos, ms);
            return true;
        }
    }
    
    UI::Host* hoveredoverpanel = getpanelat(mousepos);
    if(hoveredoverpanel){
        hoveredoverpanel->mousehover(mousepos, ms);
        return true;
    }
    return false;
}

bool InterfaceManager::click(Vec mousepos, int type)
{
    bool clickedui = false;
    if(dropdown){
        if(dropdown->checkclick(mousepos)){
            clickedui = true;
            if(type==SDL_BUTTON_LEFT)
                dropdown->leftclick(mousepos);
        }
        setdropdown(nullptr);
    }
    if(!clickedui){
        UI::Host* clickedpanel = getpanelat(mousepos);
        if(clickedpanel){
            movepaneltofront(clickedpanel);
            clickedui = true;
            movingwindow = clickedpanel;
            SDL_Rect movingwindowrect = movingwindow->getglobalrect();
            movingwindowoffset = screentoui(mousepos)-Vec(movingwindowrect.x, movingwindowrect.y);
            clickedpanel->click(mousepos, type);
        }
    }
    return clickedui;
}

void InterfaceManager::leftbuttonup(Vec mousepos)
{
    movingwindow = nullptr;
}

bool InterfaceManager::leftpressed(Vec mousepos, int mslogic)
{
    UI::Host* pressedpanel = getpanelat(mousepos);
    if(pressedpanel){
        pressedpanel->mousepress(mousepos, mslogic, SDL_BUTTON_LEFT);
    }
    if(movingwindow){
        movingwindow->move(screentoui(mousepos) - movingwindowoffset);
        return true;
    }
    return false;
}

void InterfaceManager::addpanel(UI::Host* panel)
{
    panels.emplace_back(panel);
    movepaneltofront(panel);
}

void InterfaceManager::removepanel(UI::Host* panel)
{
    if(movingwindow==panel)
        movingwindow = nullptr;
    auto it = std::find_if(panels.begin(), panels.end(), 
        [panel](const std::unique_ptr<UI::Host>& ptr){
            return ptr.get() == panel;
        });
    if(it!=panels.end())
        panels.erase(it);
}

void InterfaceManager::movepaneltofront(UI::Host* selectedpanel)
{
    auto it = std::find_if(panels.begin(), panels.end(), [selectedpanel](const std::unique_ptr<UI::Host>& ptr) {
        return ptr.get() == selectedpanel;
    });

    if (it != panels.end()) {
        auto temp = std::move(*it);
        panels.erase(it);
        panels.insert(panels.begin(), std::move(temp));
    }
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

float InterfaceManager::getlogicalscale()
{
    return uiscale;
}

void InterfaceManager::increaseuiscale()
{
    uiscale += 0.2;
    int newfontsize = 12*uiscale;
    setfontsize(newfontsize);
}

void InterfaceManager::decreaseuiscale()
{
    uiscale = std::fmax(0.4, uiscale-0.2);
    int newfontsize = 12*uiscale;
    setfontsize(newfontsize);
}

SDL_Rect InterfaceManager::uitoscreen(SDL_Rect uirect)
{
    float scale = getlogicalscale();
    SDL_Rect screenrect = {int(round(uirect.x*scale)), int(round(uirect.y*scale)), int(round(uirect.w*scale)), int(round(uirect.h*scale))};
    return screenrect;
}

SDL_Rect InterfaceManager::screentoui(SDL_Rect screenrect)
{
    float scale = getlogicalscale();
    SDL_Rect uirect = {int(round(screenrect.x/scale)), int(round(screenrect.y/scale)), int(round(screenrect.w/scale)), int(round(screenrect.h/scale))};
    return uirect;
}

Vec InterfaceManager::uitoscreen(Vec pos)
{
    float scale = getlogicalscale();
    Vec uipos = Vec(pos.x*scale, pos.y*scale);
    return uipos;
}

Vec InterfaceManager::screentoui(Vec pos)
{
    float scale = getlogicalscale();
    Vec uipos = Vec(pos.x/scale, pos.y/scale);
    return uipos;
}

namespace UI{

Element::Element(Panel* newpanel)
{
    panel = newpanel;
    ui = &panel->getui();
    game = &ui->getgame();
}

bool Element::checkclick(Vec mousepos)
{
    SDL_Rect absrect = ui->uitoscreen(getglobalrect());
	if(mousepos.x>=absrect.x && mousepos.x<=absrect.x+absrect.w && mousepos.y>=absrect.y && mousepos.y<=absrect.y+absrect.h){
		return true;
	}
    return false;
}

SDL_Rect Element::getglobalrect()
{
    SDL_Rect panelrect = panel->getglobalrect();
    return {panelrect.x+rect.x, panelrect.y+rect.y, rect.w, rect.h};
}

SDL_Rect Element::getlocalrect()
{
    return rect;
}

void Text::render(Rendering* r)
{
    SDL_Rect screenrect = ui->uitoscreen(getglobalrect());
    if(centered)
        r->rendercenteredtext(text, int(screenrect.x+screenrect.w*0.5), int(screenrect.y+screenrect.h*0.5), color, false, false, screenrect.w);
    else
        r->rendertext(text, screenrect.x, screenrect.y, color, false, false, screenrect.w);
}

void TrainIcons::render(Rendering* r)
{
    rendertrainicons(r, *ui, train.getinfo(), getglobalrect());
}

}