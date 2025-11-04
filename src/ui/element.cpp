#include "bahnhof/graphics/rendering.h"
#include "bahnhof/ui/ui.h"
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

}