#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/ui/panels.h"
#include "bahnhof/graphics/rendering.h"


InterfaceManager::InterfaceManager(Game* newgame) : game(newgame), uirendering(*this)
{
    new UI::MainPanel(this, {0,0,180,280});
}

void InterfaceManager::update(int ms)
{
    if(dropdown){
        dropdown->update(ms);
    }
    
    // TODO: Develop proper deletion system to ensure no removal during iteration
    for(auto it = panels.rbegin(); it!=panels.rend(); ++it)
        (*it)->update(ms);
}

void InterfaceManager::render(Rendering* r)
{
    int viewheight = uirendering.screentoui(r->getviewsize()).y;
    uirendering.renderscaleruler(r, 20, viewheight-20, 200);

    for(auto pit = panels.rbegin(); pit!=panels.rend(); ++pit)
        (*pit)->render(r);

    if(dropdown)
        dropdown->render(r);
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
            movingwindowoffset = uirendering.screentoui(mousepos)-Vec(movingwindowrect.x, movingwindowrect.y);
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
        movingwindow->move(uirendering.screentoui(mousepos) - movingwindowoffset);
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
    // TODO: Develop proper deletion system to ensure no removal during iteration
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
