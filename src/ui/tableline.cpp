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

TableTextLine::TableTextLine(Panel* newpanel, Table* newtable, std::string newstr) : Element(newpanel), TableLine(newpanel, newtable)
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

RouteTableLine::RouteTableLine(Panel* p, Table* t, std::string routename, int i) :
    Element(p), 
    routeindex(i),
    TableTextLine(p, t, routename), 
    Button(p, Vec(0,0))
{}

void RouteTableLine::render(Rendering* r, SDL_Rect maxarea)
{
    TableTextLine::render(r, maxarea);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    r->renderrectangle(getglobalrect(), false, false);
}

void RouteTableLine::leftclick(Vec mousepos)
{
    RouteListPanel* rlp = dynamic_cast<RouteListPanel*>(panel);
    rlp->addroutepanel(routeindex);
}

void NewRouteTableLine::leftclick(Vec mousepos)
{
    game->getgamestate().getrouting().addroute();
}

void SelectRouteTableLine::leftclick(Vec mousepos)
{
    RouteManager& routing = game->getgamestate().getrouting();
    dynamic_cast<TrainPanel*>(panel)->gettrain().route = routing.getroute(routeindex);
}

void SelectRouteTableLine::render(Rendering* r, SDL_Rect maxarea)
{
    // TODO: cheating here by rendering table text twice to get dimensions
    RouteTableLine::render(r, maxarea);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    r->renderfilledrectangle(getglobalrect(), false, false);
    RouteTableLine::render(r, maxarea);
}

OrderTableLine::OrderTableLine(Panel* p, Table* t, Route* r, int i, std::string description) :
    Element(p), 
    route(r),
    orderid(i),
    TableTextLine(p, t, description), 
    Button(p, Vec(0,0))
{}

void OrderTableLine::render(Rendering* r, SDL_Rect maxarea)
{
    uint8_t intensity = 255*(orderid==route->selectedorderid);
    SDL_Color c = {intensity, intensity, intensity, 255};
    TableTextLine::render(r, maxarea, c);
    SDL_SetRenderDrawColor(renderer,intensity,intensity,intensity,255);
    r->renderrectangle(getglobalrect(), false, false);
}

void OrderTableLine::leftclick(Vec mousepos)
{
    route->selectedorderid = orderid;
}

TrainTableLine::TrainTableLine(Panel* p, Table* t, TrainInfo traininfo, TrainManager* manager) : 
    Element(p), 
    TableLine(p, t), 
    Button(p, Vec(0,0)), 
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
    
    int iconoffset = 2*scale;
	SpriteManager& spritemanager = ui->getgame().getsprites();
    Sprite wagonicon;
    wagonicon.ported = false;
    wagonicon.zoomed = false;
    int icon_x = namerowwidth + rowoffset;
    for(WagonInfo& wagoninfo : info.wagoninfos){
        wagonicon.setspritesheet(spritemanager, wagoninfo.iconname);
        Vec iconsize = wagonicon.getsize();
        wagonicon.render(r, Vec(absrect.x+icon_x+iconsize.x*0.5, absrect.y+textpadding+namerect.h*0.5));
        icon_x += iconsize.x + iconoffset;
    }
    
    rect.h = absrect.h;
    r->renderrectangle(getglobalrect(), false, false);
}

void TrainTableLine::leftclick(Vec mousepos)
{
    trainmanager->deselectall();
    info.train->selected = true;
    
    Vec viewsize = game->getrendering().getviewsize();
    int scale = ui->getlogicalscale();
    rect = {scale*300,scale*200,scale*400,scale*200};
    new TrainPanel(ui, rect, *info.train);
}

} //end namespace UI