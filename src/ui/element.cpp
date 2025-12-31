#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/host.h"
#include "bahnhof/ui/element.h"

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

UIVec Element::getminimumsize()
{
    return {rect.w+2*padding.x, rect.h+2*padding.y};
}

UIRect Element::place(UIRect r)
{
    rect.x = r.x+getpadding().x;
    rect.y = r.y+getpadding().y;
    return rect;
}

} // namespace UI