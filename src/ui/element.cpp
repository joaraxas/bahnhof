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

bool Element::checkclick(Vec mousepos)
{
    SDL_Rect absrect = ui->getuirendering().uitoscreen(getglobalrect());
	if(mousepos.x>=absrect.x && 
        mousepos.x<=absrect.x+absrect.w && 
        mousepos.y>=absrect.y && 
        mousepos.y<=absrect.y+absrect.h)
    {
        return true;
    }
    return false;
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