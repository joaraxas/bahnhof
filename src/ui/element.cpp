#include "bahnhof/graphics/rendering.h"
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/host.h"
#include "bahnhof/ui/element.h"
#include "bahnhof/ui/decoration.h"

namespace UI{

Element::Element(Host* newpanel)
{
    panel = newpanel;
    ui = &panel->getui();
    game = &ui->getgame();
}

bool Element::checkclick(UIVec mousepos)
{
    return getglobalrect().contains(mousepos);
}

UIRect Element::getglobalrect()
{
    UIRect panelrect = panel->getglobalrect();
    return {panelrect.x+rect.x, panelrect.y+rect.y, rect.w, rect.h};
}

UIRect Element::getlocalrect()
{
    return rect;
}

void Element::place(UIRect r)
{
    rect.x = r.x+3;
    rect.y = r.y+2;
}


Element* Layout::addelement(Element* el)
{
    elements.push_back(el);
    return el;
}

UIVec Layout::organize()
{
    UIVec sz{getminimumsize()};
    UIRect placerect{0, 0, sz.x, sz.y};
    place(placerect);
    return sz;
}

void HBox::place(UIRect placerect)
{
    rect = placerect;
    Coord totminwidth = std::accumulate(minwidths.begin(), minwidths.end(), Coord{0});
    Coord extrawidth = std::floor((placerect.w-totminwidth)*(1./elements.size()));
    for(int i=0; i<elements.size(); ++i){
        auto el = elements[i];
        placerect.w = minwidths[i]+extrawidth;
        if(i==elements.size()) placerect.w = rect.w-placerect.x;
        el->place(placerect);
        placerect.x += placerect.w;
    }
}

UIVec HBox::getminimumsize()
{
    UIVec sz{0,0};
    minwidths.clear();
    for(auto el : elements){
        auto r = el->getminimumsize();
        sz.x += r.x;
        sz.y = std::max(sz.y, r.y);
        minwidths.push_back(r.x);
    }
    return sz;
}

void VBox::place(UIRect placerect)
{
    rect = placerect;
    Coord totminheight = std::accumulate(minheights.begin(), minheights.end(), Coord{0});
    Coord extraheight = std::floor((placerect.h-totminheight)*(1./elements.size()));
    for(int i=0; i<elements.size(); ++i){
        auto el = elements[i];
        placerect.h = minheights[i]+extraheight;
        if(i==elements.size()) placerect.h = rect.h-placerect.y;
        el->place(placerect);
        placerect.y += placerect.h;
    }
}

UIVec VBox::getminimumsize()
{
    UIVec sz{0,0};
    minheights.clear();
    for(auto el : elements){
        auto r = el->getminimumsize();
        sz.y += r.y;
        sz.x = std::max(sz.x, r.x);
        minheights.push_back(r.y);
    }
    return sz;
}

}