#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/ui/tablelines.h"
#include "bahnhof/ui/decoration.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/buildings/buildingtypes.h"

namespace UI{

TableLine::TableLine(Host* p, Table* t, std::string s) : 
    Element(p),
    table(t),
    str(s)
{
    UIRect tablerect = table->getlocalrect();
    rect = {tablerect.x, tablerect.y, tablerect.w, 20};
}

UIRect TableLine::getglobalrect()
{
    UIRect tablepos = table->getglobalrect();
    return {tablepos.x+rect.x, tablepos.y+rect.y, rect.w, rect.h};
}

void TableLine::render(Rendering* r, UIRect maxarea, TextStyle style, Coord xmargin, Coord ymargin)
{
    rect.x = maxarea.x;
    rect.y = maxarea.y;
    UIRect textrect = ui->getuirendering().rendertext(r, str, getlocalrect(), style, false, xmargin, ymargin);
    rect.h = textrect.h;
}

TrainTableLine::TrainTableLine(Host* p, Table* t, TrainInfo traininfo) : 
    TableLine(p, t, traininfo.name), 
    info(traininfo)
{}

void TrainTableLine::render(
    Rendering* r, 
    UIRect maxarea, 
    TextStyle style, 
    Coord xmargin, 
    Coord ymargin
    )
{
    rect = maxarea;
    Coord textpadding = 5;
    Coord namerowwidth = 60;
    UIRect namerect = getlocalrect();
    namerect.w = namerowwidth;
    namerect.x = xmargin;
    
    namerect = ui->getuirendering().rendertext(
        r, str, namerect, style, false, textpadding, ymargin);
    rect.h = namerect.h;
    
    UIRect trainiconrect = getlocalrect();
    trainiconrect = {trainiconrect.x+xmargin+namerowwidth, 
                     trainiconrect.y+ymargin, 
                     trainiconrect.w-namerowwidth-2*xmargin, 
                     namerect.h-2*ymargin};
    // The local rectangle works here because we are rendering to a 
    // separate table target
    rendertrainicons(r, *ui, info.wagoninfos, trainiconrect);
}

PurchaseOptionTableLine::PurchaseOptionTableLine(Host* p, Table* t, sprites::name iconname, std::string name, float cost) : 
    TableLine(p, t, name),
    price(cost)
{
    SpriteManager& sprites = game->getsprites();
    icon.setspritesheet(sprites, iconname);
    icon.ported = false;
}

void PurchaseOptionTableLine::render(
    Rendering* r, 
    UIRect maxarea, 
    TextStyle style, 
    Coord xmargin, 
    Coord ymargin)
{
    rect = maxarea;
    UIRendering& uiren = ui->getuirendering();
    Vec screeniconsize = icon.getsize();
    UIVec uiiconsize = uiren.screentoui(screeniconsize);
    Coord rowoffset = ymargin;
    Coord textpadding = 5;
    Coord pricerowwidth = 50;
    Coord iconwidth = uiiconsize.x + xmargin + textpadding;
    Coord namerowwidth = getlocalrect().w - iconwidth - pricerowwidth;
    
    UIRect namerect = getlocalrect();
    namerect.w = namerowwidth;
    namerect.x += iconwidth;
    namerect.y += uiiconsize.y * 0.5 - 12 * 0.5 - 1;
    namerect = uiren.rendertext(
        r, str, namerect, style, false, textpadding, rowoffset);
    UIRect pricerect = getlocalrect();
    pricerect.x = pricerect.x + pricerect.w - pricerowwidth;
    pricerect.w = pricerowwidth;
    pricerect.y = namerect.y;
    pricerect = uiren.rendertext(
                    r, 
                    std::to_string(price)+" Fr", 
                    pricerect, 
                    style, 
                    false, 
                    textpadding, 
                    rowoffset);
    rect.h = std::max(
        std::max(namerect.h, pricerect.h), 
            uiiconsize.y+2*rowoffset);
    UIRect uiiconrect = getlocalrect();
    uiiconrect.x += xmargin + textpadding;
    uiiconrect.y += rowoffset;
    SDL_Rect screeniconrect = uiren.uitoscreen(uiiconrect);
    icon.render(r, 
                Vec(screeniconrect.x+screeniconsize.x*0.5, 
                screeniconrect.y+screeniconsize.y*0.5)
    );
}

} //end namespace UI