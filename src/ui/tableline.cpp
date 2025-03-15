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
    return {tablepos.x+rect.x, tablepos.y+rect.y, rect.w, rect.h};
}

TableTextLine::TableTextLine(Panel* newpanel, Table* newtable, std::string newstr) : TableLine(newpanel, newtable)
{
    str = newstr;
}

void TableTextLine::render(Rendering* r, SDL_Rect maxarea, TextStyle style)
{
    rect = maxarea;
    SDL_Rect textrect = ui->rendertext(r, str, getglobalrect(), style);
    if(!nicetracks)
        r->renderrectangle(ui->uitoscreen(getglobalrect()), false, false);
    rect.h = textrect.h;
}

RouteTableLine::RouteTableLine(Panel* p, Table* t, std::string routename) :
    TableTextLine(p, t, routename)
{}

void RouteTableLine::render(Rendering* r, SDL_Rect maxarea)
{
    TableTextLine::render(r, maxarea);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    r->renderrectangle(ui->uitoscreen(getglobalrect()), false, false);
}

OrderTableLine::OrderTableLine(Panel* p, Table* t, bool sel, int i, std::string description) :
    selected(sel),
    orderid(i),
    TableTextLine(p, t, description)
{}

void OrderTableLine::render(Rendering* r, SDL_Rect maxarea)
{
    uint8_t intensity = 255*selected;
    TextStyle style;
    if(selected)
        style = Highlighted;
    else
        style = Info;
    TableTextLine::render(r, maxarea, style);
    SDL_SetRenderDrawColor(renderer,intensity,intensity,intensity,255);
    r->renderrectangle(ui->uitoscreen(getglobalrect()), false, false);
}

TrainTableLine::TrainTableLine(Panel* p, Table* t, TrainInfo traininfo, TrainManager* manager) : 
    TableLine(p, t), 
    info(traininfo),
    trainmanager(manager)
{}

void TrainTableLine::render(Rendering* r, SDL_Rect maxarea)
{
    rect = maxarea;
    TextStyle style = Info;
    if(info.train->selected)
        style = Highlighted;
    Uint8 intensity = 255*info.train->selected;
    int rowoffset = 2;
    int textpadding = 5;
    int namerowwidth = 60;
    SDL_Rect namerect = getglobalrect();
    namerect = {namerect.x+textpadding, namerect.y+rowoffset, namerowwidth-2*textpadding, 100};
    SDL_SetRenderDrawColor(renderer,intensity,intensity,intensity,255);
    
    namerect = ui->rendertext(r, info.name, namerect, style);
    rect.h = namerect.h+2*rowoffset;
    
    SDL_Rect trainiconrect = getglobalrect();
    trainiconrect = {trainiconrect.x+namerowwidth+textpadding, 
                              trainiconrect.y+rowoffset, 
                              trainiconrect.w-namerowwidth-2*textpadding, 
                              namerect.h};
    rendertrainicons(r, *ui, info, trainiconrect);
    
    r->renderrectangle(ui->uitoscreen(getglobalrect()), false, false);
}

SDL_Rect rendertrainicons(Rendering* r, InterfaceManager& ui, TrainInfo info, SDL_Rect maxrect)
{
    SDL_Rect screenrect = ui.uitoscreen(maxrect);
    auto scale = ui.getuiscale();
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

} //end namespace UI