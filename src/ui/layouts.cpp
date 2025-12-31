#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/layouts.h"

namespace UI{

Layout::Layout(Host* host) :
    Element(host)
{
    setpadding({0,0});
}

void Layout::addelements(std::vector<Element*> els)
{
    for(auto el : els){
        addelement(el);
    }
}

void Box::addelement(Element* el)
{
    elements.push_back(el);
    minsizes.push_back(el->getminimumsize());
    if(el->resizable_x())
        ++numresizableelements_x;
    if(el->resizable_y())
        ++numresizableelements_y;
}

UIRect addpadding(UIRect uprect, UIVec padding)
{
    return {uprect.x - padding.x, 
            uprect.y - padding.y, 
            uprect.w + 2*padding.x, 
            uprect.h + 2*padding.y};
}

UIRect removepadding(UIRect uprect, UIVec padding)
{
    return {uprect.x + padding.x, 
            uprect.y + padding.y, 
            uprect.w - 2*padding.x, 
            uprect.h - 2*padding.y};
}

UIRect expandtofit(UIRect rect1, UIVec minsize)
{
    rect1.w = std::max(rect1.w, minsize.x);
    rect1.h = std::max(rect1.h, minsize.y);
    return rect1;
}


UIVec HBox::getminimumsize()
{
    UIVec sz{0,0};
    for(auto minsize : minsizes){
        sz.x += minsize.x;
        sz.y = std::max(sz.y, minsize.y);
    }
    sz += 2*getpadding();
    return sz;
}

UIRect HBox::place(UIRect placerect)
{
    UIVec minsz{getminimumsize()};
    UIRect newpaddedrect = expandtofit(placerect, minsz);
    rect = removepadding(newpaddedrect, getpadding());

    Coord extrawidth{0};
    if(numresizableelements_x>0 && placerect.w>minsz.x)
        extrawidth = (placerect.w - minsz.x)/numresizableelements_x;

    Coord xdiff{0};
    for(int i = 0; i<elements.size(); ++i){
        Coord elwidth = minsizes[i].x;
        if(elements[i]->resizable_x())
            elwidth += extrawidth;
        if(i==elements.size()-1)
            elwidth = rect.w-xdiff;
        elements[i]->place({rect.x+xdiff, rect.y, elwidth, rect.h});
        xdiff += elwidth;
    }

    return newpaddedrect;
}


UIVec VBox::getminimumsize()
{
    UIVec sz{0,0};
    for(auto minsize : minsizes){
        sz.y += minsize.y;
        sz.x = std::max(sz.x, minsize.x);
    }
    sz += 2*getpadding();
    return sz;
}

UIRect VBox::place(UIRect placerect)
{
    UIVec minsz{getminimumsize()};
    UIRect newpaddedrect = expandtofit(placerect, minsz);
    rect = removepadding(newpaddedrect, getpadding());

    Coord extraheight{0};
    if(numresizableelements_y>0 && placerect.h>minsz.y)
        extraheight = (placerect.h - minsz.y)/numresizableelements_y;

    Coord ydiff{0};
    for(int i = 0; i<elements.size(); ++i){
        Coord elheight = minsizes[i].y;
        if(elements[i]->resizable_y())
            elheight += extraheight;
        if(i==elements.size()-1)
            elheight = rect.h-ydiff;
        elements[i]->place({rect.x, rect.y+ydiff, rect.w, elheight});
        ydiff += elheight;
    }

    return newpaddedrect;
}


} // namespace UI