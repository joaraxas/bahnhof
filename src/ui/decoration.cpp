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
    ui->getuirendering().rendertext(r, text, getglobalrect(), style, centered);
}

void TrainIcons::render(Rendering* r)
{
    iconrects = rendertrainicons(r, *ui, train.getinfo(), getglobalrect());
}

void TrainIcons::mousehover(Vec pos, int ms)
{
    return;
}
    
void TrainIcons::leftclick(Vec mousepos)
{
    int wheretosplit = 0;
    mousepos = ui->getuirendering().screentoui(mousepos);
    for(int iRect=0; iRect<iconrects.size(); iRect++){
        SDL_Rect& rect = iconrects[iRect];
	    if(mousepos.x>=rect.x && mousepos.x<=rect.x+rect.w && mousepos.y>=rect.y && mousepos.y<=rect.y+rect.h){
            wheretosplit = iRect + 1;
            break;
        }
    }
    if(!train.gasisforward){
        wheretosplit = iconrects.size()-wheretosplit+1;
    }
    train.split(wheretosplit, nullptr);
}

std::vector<SDL_Rect> rendertrainicons(Rendering* r, InterfaceManager& ui, TrainInfo info, SDL_Rect maxrect)
{
    SDL_Rect screenrect = ui.getuirendering().uitoscreen(maxrect);
    auto scale = ui.getuirendering().getuiscale();
    int iconoffset = 2*scale;
	SpriteManager& spritemanager = ui.getgame().getsprites();
    Icon wagonicon;
    wagonicon.ported = false;
    int icon_x = 0;
    std::vector<SDL_Rect> iconuirects;
    for(WagonInfo& wagoninfo : info.wagoninfos){
        wagonicon.setspritesheet(spritemanager, wagoninfo.iconname);
        Vec iconsize = wagonicon.getsize();
        SDL_Rect iconrect = {screenrect.x+icon_x, int(screenrect.y+screenrect.h*0.5-iconsize.y*0.5), int(iconsize.x), int(iconsize.y)};
        wagonicon.render(r, Vec(iconrect.x+iconsize.x*0.5, iconrect.y+iconsize.y*0.5));
        iconuirects.push_back(ui.getuirendering().screentoui(iconrect));
        icon_x += iconsize.x + iconoffset;
    }
    return iconuirects;
}

}