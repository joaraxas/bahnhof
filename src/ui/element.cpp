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
    rect.x = r.x;
    rect.y = r.y;
}


Element* Layout::addelement(Element* el)
{
    elements.push_back(el);
    return el;
}

void Layout::organize()
{
    rect = UIRect{rect.x, rect.y, 2, 0};
    for(auto el : elements){
        auto r = el->getlocalrect();
        rect.w += r.w+2;
        rect.h = std::max(rect.h, r.h+6);
    }

    UIRect placerect{rect.x+2, rect.y+3, rect.w-4, rect.h-6};
    for(auto el : elements){
        el->place(placerect);
        if(dynamic_cast<Layout*>(el))
            dynamic_cast<Layout*>(el)->organize();
        placerect.x += el->getlocalrect().w + 2;
    }
}

}