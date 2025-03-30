#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/ui/decoration.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/train.h"

namespace UI{

TableLine::TableLine(Host* newpanel, Table* newtable) : Element(newpanel)
{
    table = newtable;
    SDL_Rect tablerect = table->getlocalrect();
    rect = {tablerect.x, tablerect.y, tablerect.w, 20};
}

SDL_Rect TableLine::getglobalrect()
{
    SDL_Rect tablepos = table->getglobalrect();
    return {tablepos.x+rect.x, tablepos.y+rect.y, rect.w, rect.h};
}

TableTextLine::TableTextLine(Host* newpanel, Table* newtable, std::string newstr) : TableLine(newpanel, newtable)
{
    str = newstr;
    SDL_Rect textrect = ui->getuirendering().gettextsize(str, rect, 1, 1);
    rect.h = textrect.h;
}

void TableTextLine::render(Rendering* r, SDL_Rect maxarea, TextStyle style)
{
    rect.x = maxarea.x;
    rect.y = maxarea.y;
    ui->getuirendering().rendertext(r, str, getlocalrect(), style);
}

RouteTableLine::RouteTableLine(Host* p, Table* t, std::string routename) :
    TableTextLine(p, t, routename)
{}

void RouteTableLine::render(Rendering* r, SDL_Rect maxarea)
{
    TableTextLine::render(r, maxarea);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    r->renderrectangle(ui->getuirendering().uitoscreen(getlocalrect()), false, false);
}

OrderTableLine::OrderTableLine(Host* p, Table* t, bool sel, int i, std::string description) :
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
    r->renderrectangle(ui->getuirendering().uitoscreen(getlocalrect()), false, false);
}

TrainTableLine::TrainTableLine(Host* p, Table* t, TrainInfo traininfo, TrainManager* manager) : 
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
    SDL_Rect namerect = getlocalrect();
    namerect.w = namerowwidth;
    SDL_SetRenderDrawColor(renderer,intensity,intensity,intensity,255);
    
    namerect = ui->getuirendering().rendertext(r, info.name, namerect, style, false, textpadding, rowoffset);
    rect.h = namerect.h;
    
    SDL_Rect trainiconrect = getlocalrect();
    trainiconrect = {trainiconrect.x+namerowwidth+textpadding, 
                              trainiconrect.y+rowoffset, 
                              trainiconrect.w-namerowwidth-2*textpadding, 
                              namerect.h-2*rowoffset};
    rendertrainicons(r, *ui, info, trainiconrect);
    
    r->renderrectangle(ui->getuirendering().uitoscreen(getlocalrect()), false, false);
}

} //end namespace UI