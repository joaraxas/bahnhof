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
    TrainInfo traininfo = train.getinfo();
    iconrects = rendertrainicons(r, *ui, traininfo.wagoninfos, getglobalrect(), rendersplitafterwagonid);
    rendersplitafterwagonid = -1;
}

void TrainIcons::mousehover(Vec pos, int ms)
{
    rendersplitafterwagonid = std::fmin(getwagonidatmousepos(pos), train.getinfo().wagoninfos.size()-2);
}

void TrainIcons::leftclick(Vec mousepos)
{
    int wagonid = getwagonidatmousepos(mousepos);
    if(wagonid<0) return;
    int wheretosplit = 0;
    if(!train.gasisforward){
        wheretosplit = iconrects.size()-wagonid-1;
    }
    else{
        wheretosplit = wagonid + 1;
    }
    train.split(wheretosplit, nullptr);
}

int TrainIcons::getwagonidatmousepos(Vec mousepos)
{
    mousepos = ui->getuirendering().screentoui(mousepos);
    for(int iRect=0; iRect<iconrects.size(); iRect++){
        SDL_Rect& rect = iconrects[iRect];
	    if(mousepos.x>=rect.x && mousepos.x<=rect.x+rect.w && mousepos.y>=rect.y && mousepos.y<=rect.y+rect.h){
            return iRect;
        }
    }
    return -1;
}

std::vector<SDL_Rect> rendertrainicons(Rendering* r, InterfaceManager& ui, std::vector<WagonInfo>& wagoninfos, SDL_Rect maxrect, int splitid)
{
    SDL_Rect screenrect = ui.getuirendering().uitoscreen(maxrect);
    auto scale = ui.getuirendering().getuiscale();
    int iconoffset = 2*scale;
    int splitoffset = 0;
	SpriteManager& spritemanager = ui.getgame().getsprites();
    Icon wagonicon;
    wagonicon.ported = false;
    int icon_x = 0;
    std::vector<SDL_Rect> iconuirects;
    for(int iWagon = 0; iWagon<wagoninfos.size(); iWagon++){
        WagonInfo& wagoninfo = wagoninfos[iWagon];
        wagonicon.setspritesheet(spritemanager, wagoninfo.iconname);
        if(splitid>=0 && iWagon>splitid)
            splitoffset = 5*scale;
        Vec iconsize = wagonicon.getsize();
        wagonicon.render(r, Vec(screenrect.x+icon_x+iconsize.x*0.5+splitoffset, screenrect.y+screenrect.h*0.5));
        SDL_Rect iconrect = {screenrect.x+icon_x, screenrect.y, int(iconsize.x+iconoffset), screenrect.h};
        iconuirects.push_back(ui.getuirendering().screentoui(iconrect));
        icon_x += iconsize.x + iconoffset;
    }
    return iconuirects;
}

}