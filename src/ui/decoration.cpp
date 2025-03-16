#include<iostream>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/decoration.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/rollingstock/train.h"

namespace UI{

void Text::render(Rendering* r)
{
    SDL_Rect screenrect = ui->getuirendering().uitoscreen(getglobalrect());
    if(centered)
        r->rendercenteredtext(text, int(screenrect.x+screenrect.w*0.5), int(screenrect.y+screenrect.h*0.5), color, false, false, screenrect.w);
    else
        r->rendertext(text, screenrect.x, screenrect.y, color, false, false, screenrect.w);
}

void TrainIcons::render(Rendering* r)
{
    rendertrainicons(r, *ui, train.getinfo(), getglobalrect());
}

SDL_Rect rendertrainicons(Rendering* r, InterfaceManager& ui, TrainInfo info, SDL_Rect maxrect)
{
    SDL_Rect screenrect = ui.getuirendering().uitoscreen(maxrect);
    auto scale = ui.getuirendering().getuiscale();
    int iconoffset = 2*scale;
	SpriteManager& spritemanager = ui.getgame().getsprites();
    Icon wagonicon;
    wagonicon.ported = false;
    int icon_x = 0;
    for(WagonInfo& wagoninfo : info.wagoninfos){
        wagonicon.setspritesheet(spritemanager, wagoninfo.iconname);
        Vec iconsize = wagonicon.getsize();
        wagonicon.render(r, Vec(screenrect.x+icon_x+iconsize.x*0.5, screenrect.y+screenrect.h*0.5));
        icon_x += iconsize.x + iconoffset;
    }
    return maxrect;
}

}