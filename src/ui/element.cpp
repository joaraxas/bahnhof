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
    rect.x = r.x+2;
    rect.y = r.y+3;
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

void Layout::place(UIRect placerect)
{
    rect = placerect;
    Coord totminsize = std::accumulate(minsizes.begin(), minsizes.end(), Coord{0});
    Coord extrawidth = std::floor((placerect.w-totminsize)*(1./elements.size()));
    for(int i=0; i<elements.size(); ++i){
        auto el = elements[i];
        placerect.w = minsizes[i]+extrawidth;
        if(i==elements.size()) placerect.w = rect.w-placerect.x;
        el->place(placerect);
        placerect.x += placerect.w;
    }
}

UIVec Layout::getminimumsize()
{
    UIVec sz{0,0};
    minsizes.clear();
    for(auto el : elements){
        auto r = el->getminimumsize();
        sz.x += r.x;
        sz.y = std::max(sz.y, r.y);
        minsizes.push_back(r.x);
    }
    return sz;
}

}