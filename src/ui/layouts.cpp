#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/layouts.h"

namespace UI{

Layout::Layout(Host* host, std::initializer_list<Element*> els) :
    Element(host)
{
    setpadding({0,0});
    addelements(els);
}

void Layout::addelements(std::vector<Element*> els)
{
    for(auto el : els)
        addelement(el);
}

void Layout::addelement(Element* el)
{
    elements.push_back(el);
}

void HBox::addelement(Element* el)
{
    Layout::addelement(el);
    auto r = el->getminimumsize();
    minwidths.push_back(r.x);
    if(el->resizable_x())
        ++numresizableelements_x;
    if(el->resizable_y())
        anyresizableelement_y = true;
}

UIVec HBox::getminimumsize()
{
    UIVec sz{0,0};
    minwidths.clear();
    numresizableelements_x = 0;
    anyresizableelement_y = false;
    for(auto el : elements){
        auto r = el->getminimumsize();
        sz.x += r.x;
        sz.y = std::max(sz.y, r.y);
        minwidths.push_back(r.x);
        if(el->resizable_x())
            ++numresizableelements_x;
        if(!anyresizableelement_y && el->resizable_y())
            anyresizableelement_y = true;
    }
    sz += 2*getpadding();
    return sz;
}

UIRect HBox::place(UIRect placerect)
{
    UIVec sz{getminimumsize()};
    placerect = {placerect.x, 
                placerect.y, 
                std::max(sz.x,placerect.w), 
                std::max(sz.y,placerect.h)};
    rect = placerect;
    placerect.x += getpadding().x;
    placerect.y += getpadding().y;
    placerect.w -= 2*getpadding().x;
    placerect.h -= 2*getpadding().y;

    Coord totminwidth = std::accumulate(
        minwidths.begin(), minwidths.end(), Coord{0}
    );
    Coord extrawidth{0};
    if(numresizableelements_x>0)
        extrawidth = std::floor(
            (placerect.w-totminwidth)*(1./numresizableelements_x)
        );

    for(int i=0; i<elements.size(); ++i){
        auto el = elements[i];
        placerect.w = minwidths[i];
        if(el->resizable_x())
            placerect.w += extrawidth;
        if(i==elements.size()) placerect.w = rect.w-placerect.x;
        el->place(placerect);
        placerect.x += placerect.w;
    }

    return rect;
}


void VBox::addelement(Element* el)
{
    Layout::addelement(el);
    auto r = el->getminimumsize();
    minheights.push_back(r.y);
    if(el->resizable_y())
        ++numresizableelements_y;
    if(el->resizable_x())
        anyresizableelement_x = true;
}

UIVec VBox::getminimumsize()
{
    UIVec sz{0,0};
    minheights.clear();
    numresizableelements_y = 0;
    anyresizableelement_x = false;
    for(auto el : elements){
        auto r = el->getminimumsize();
        sz.y += r.y;
        sz.x = std::max(sz.x, r.x);
        minheights.push_back(r.y);
        if(el->resizable_y())
            ++numresizableelements_y;
        if(!anyresizableelement_x && el->resizable_x())
            anyresizableelement_x = true;
    }
    sz += 2*getpadding();
    return sz;
}

UIRect VBox::place(UIRect placerect)
{
    UIVec sz{getminimumsize()};
    placerect = {placerect.x, 
                placerect.y, 
                std::max(sz.x,placerect.w), 
                std::max(sz.y,placerect.h)};
    rect = placerect;
    placerect.x += getpadding().x;
    placerect.y += getpadding().y;
    placerect.w -= 2*getpadding().x;
    placerect.h -= 2*getpadding().y;

    Coord totminheight = std::accumulate(
        minheights.begin(), minheights.end(), Coord{0}
    );
    Coord extraheight{0};
    if(numresizableelements_y>0){
        extraheight = std::floor(
            (placerect.h-totminheight)*(1./numresizableelements_y)
        );
    }
    
    for(int i=0; i<elements.size(); ++i){
        auto el = elements[i];
        placerect.h = minheights[i];
        if(el->resizable_y())
            placerect.h += extraheight;
        if(i==elements.size())
            placerect.h = rect.h-placerect.y;
        el->place(placerect);
        placerect.y += placerect.h;
    }

    return rect;
}

} // namespace UI