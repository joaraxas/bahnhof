#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/routing/routing.h"

namespace UI{

TableLine::TableLine(Panel* newpanel, Table* newtable) : Element(newpanel)
{
    table = newtable;
    SDL_Rect tablerect = table->getlocalrect();
    rect = {tablerect.x, tablerect.y, 100, 20};
}

SDL_Rect TableLine::getglobalrect()
{
    SDL_Rect tablepos = table->getglobalrect();
    return {rect.x+int(tablepos.x), rect.y+int(tablepos.y), rect.w, rect.h};
}

TableTextLine::TableTextLine(Panel* newpanel, Table* newtable, std::string newstr) : TableLine(newpanel, newtable)
{
    str = newstr;
}

void TableTextLine::render(Rendering* r, SDL_Rect maxarea, SDL_Color color)
{
    rect = maxarea;
    SDL_Rect globrect = getglobalrect();
    SDL_Rect textrect = r->rendertext(str, globrect.x, globrect.y, color, false, false, globrect.w);
    rect.h = textrect.h;
}

RouteTableLine::RouteTableLine(Panel* p, Table* t, std::string routename) :
    TableTextLine(p, t, routename)
{}

void RouteTableLine::render(Rendering* r, SDL_Rect maxarea)
{
    TableTextLine::render(r, maxarea);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    r->renderrectangle(getglobalrect(), false, false);
}

OrderTableLine::OrderTableLine(Panel* p, Table* t, Route* r, int i, std::string description) :
    route(r),
    orderid(i),
    TableTextLine(p, t, description)
{}

void OrderTableLine::render(Rendering* r, SDL_Rect maxarea)
{
    uint8_t intensity = 255*(orderid==route->selectedorderid);
    SDL_Color c = {intensity, intensity, intensity, 255};
    TableTextLine::render(r, maxarea, c);
    SDL_SetRenderDrawColor(renderer,intensity,intensity,intensity,255);
    r->renderrectangle(getglobalrect(), false, false);
}

TrainTableLine::TrainTableLine(Panel* p, Table* t, TrainInfo traininfo, TrainManager* manager) : 
    TableLine(p, t), 
    info(traininfo),
    trainmanager(manager)
{}

void TrainTableLine::render(Rendering* r, SDL_Rect maxarea)
{
    rect = maxarea;
    auto scale = ui->getlogicalscale();
    Uint8 intensity = 255*info.train->selected;
    SDL_Rect absrect = getglobalrect();
    int rowoffset = 3*scale;
    int textpadding = 1*scale;
    int namerowwidth = 40*scale;
    SDL_SetRenderDrawColor(renderer,intensity,intensity,intensity,255);
    
    SDL_Rect namerect = r->rendertext(info.name, absrect.x+rowoffset, absrect.y+textpadding, {intensity, intensity, intensity, 255}, false, false, namerowwidth-rowoffset);
    absrect.h = namerect.h + 2*textpadding;
    
    SDL_Rect trainiconrect = {absrect.x+rowoffset+namerowwidth+textpadding, 
                              absrect.y+textpadding, 
                              absrect.w-namerowwidth-rowoffset-2*textpadding, 
                              namerect.h};
    rendertrainicons(r, *ui, info, trainiconrect);
    
    rect.h = absrect.h;
    r->renderrectangle(getglobalrect(), false, false);
}

SDL_Rect rendertrainicons(Rendering* r, InterfaceManager& ui, TrainInfo info, SDL_Rect maxrect)
{
    auto scale = ui.getlogicalscale();
    int iconoffset = 2*scale;
	SpriteManager& spritemanager = ui.getgame().getsprites();
    Sprite wagonicon;
    wagonicon.ported = false;
    wagonicon.zoomed = false;
    int icon_x = 0;
    for(WagonInfo& wagoninfo : info.wagoninfos){
        wagonicon.setspritesheet(spritemanager, wagoninfo.iconname);
        Vec iconsize = wagonicon.getsize();
        wagonicon.render(r, Vec(maxrect.x+icon_x+iconsize.x*0.5, maxrect.y+maxrect.h*0.5));
        icon_x += iconsize.x + iconoffset;
    }
    return maxrect;
}

} //end namespace UI