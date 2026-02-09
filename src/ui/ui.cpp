#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/ui/panels.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"

using namespace UI;

InterfaceManager::InterfaceManager(Game* newgame) : 
    game(newgame), uirendering(*this), movingwindowoffset{0,0}, 
    tooltip(game->getinputmanager(), uirendering)
{
    new UI::MainPanel(this);
}

InterfaceManager::~InterfaceManager() {}

void InterfaceManager::update(int ms)
{
    cleanup();

    if(dropdown){
        dropdown->update(ms);
    }
    
    for(auto& panel : panels)
        panel->update(ms);
        
    cleanup();
}

void InterfaceManager::render(Rendering* r)
{
    cleanup();

    UI::Coord viewheight = uirendering.getuiview().h;
    uirendering.renderscaleruler(r, 20, viewheight-20, 200);

    for(auto pit = panels.rbegin(); pit!=panels.rend(); ++pit)
        (*pit)->render(r);

    if(dropdown)
        dropdown->render(r);
    
    tooltip.render(r);
}

UI::Host* InterfaceManager::getpanelat(UIVec uipos)
{
    cleanup();
    
    for(auto& panel: panels){
        if(panel->getglobalrect().contains(uipos))
            return panel.get();
    }
    return nullptr;
}

bool InterfaceManager::mousehover(Vec mousepos, int ms)
{
    UIVec uimpos = uirendering.screentoui(mousepos);
    if(dropdown){
        if(dropdown->checkclick(uimpos)){
            dropdown->mousehover(uimpos, ms);
            return true;
        }
    }
    
    UI::Host* hoveredoverpanel = getpanelat(uimpos);
    if(hoveredoverpanel){
        hoveredoverpanel->mousehover(uimpos, ms);
        return true;
    }
    return false;
}

bool InterfaceManager::click(Vec mousepos, int type)
{
    UIVec uimpos = uirendering.screentoui(mousepos);
    bool clickedui = false;
    if(dropdown){
        if(dropdown->checkclick(uimpos)){
            clickedui = true;
            if(type==SDL_BUTTON_LEFT)
                dropdown->leftclick(uimpos);
        }
        setdropdown(nullptr);
    }
    if(!clickedui){
        UI::Host* clickedpanel = getpanelat(uimpos);
        if(clickedpanel){
            movepaneltofront(clickedpanel);
            clickedui = true;
            clickedpanel->click(uimpos, type);
            if(clickedpanel->usermovable()){
                movingwindow = clickedpanel;
                UI::UIRect movingwindowrect = movingwindow->getglobalrect();
                movingwindowoffset = uimpos
                    - UI::UIVec{movingwindowrect.x, movingwindowrect.y};
            }
        }
    }
    cleanup();
    return clickedui;
}

bool InterfaceManager::scroll(Vec mousepos, int distance)
{
    UIVec uimpos = uirendering.screentoui(mousepos);
    bool clickedui = false;
    if(dropdown){
        if(dropdown->checkclick(uimpos)){
            clickedui = true;
            dropdown->scroll(uimpos, distance);
        }
    }
    if(!clickedui){
        UI::Host* clickedpanel = getpanelat(uimpos);
        if(clickedpanel){
            clickedui = true;
            clickedpanel->scroll(uimpos, distance);
        }
    }
    cleanup();
    return clickedui;
}

bool InterfaceManager::leftbuttonup(Vec mousepos)
{
    movingwindow = nullptr;
    UI::UIVec uimpos = uirendering.screentoui(mousepos);

    if(dropdown && dropdown->checkclick(uimpos))
        return true;
    if(getpanelat(uimpos))
        return true;
    return false;
}

bool InterfaceManager::leftpressed(Vec mousepos, int mslogic)
{
    UIVec uimpos = uirendering.screentoui(mousepos);
    UI::Host* pressedpanel = getpanelat(uimpos);
    if(pressedpanel){
        pressedpanel->mousepress(uimpos, mslogic, SDL_BUTTON_LEFT);
    }
    if(movingwindow){
        movingwindow->moveto(uimpos - movingwindowoffset);
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
    panelstodelete.push_back(panel);
}

void InterfaceManager::cleanup()
{
    for(auto panel: panelstodelete){
        if(movingwindow==panel)
            movingwindow = nullptr;
        auto it = std::find_if(panels.begin(), panels.end(), 
            [panel](const std::unique_ptr<UI::Host>& ptr){
                return ptr.get() == panel;
            });
        if(it!=panels.end())
            panels.erase(it);
    }
    panelstodelete.clear();
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

UI::UIVec InterfaceManager::findfreespace(const UI::UIVec size)
{
    UIRect view = uirendering.getuiview();
    placepanelspos += {30,30};
    if(placepanelspos.x+size.x > view.x+view.w)
        placepanelspos.x = 10;
    if(placepanelspos.y+size.y > view.y+view.h)
        placepanelspos.y = 10;
    
    return placepanelspos;
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

void InterfaceManager::handlewindowsizechange()
{
    const auto view = uirendering.getuiview();
    for(auto& panel : panels){
        panel->conformtorect(view);
    }
}

void InterfaceManager::addtooltip(std::string tip)
{
    tooltip.add(tip);
}