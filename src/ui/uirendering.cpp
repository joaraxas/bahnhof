#include "bahnhof/ui/ui.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/common/gamestate.h"

using namespace UI;

UIRendering::UIRendering(InterfaceManager& newui) : ui(newui)
{
    setuiscale(getlogicalscale());
}

void UIRendering::rendertexture(
    Rendering* r, 
    SDL_Texture* tex, 
    UIRect rect, 
    SDL_Rect* srcrect, 
    Angle angle, 
    bool ported, 
    bool originiscenter, 
    int centerx, 
    int centery)
{
    SDL_Rect screenrect = uitoscreen(rect);
    r->rendertexture(
        tex, 
        &screenrect, 
        srcrect, 
        angle, 
        ported, 
        false, 
        originiscenter, 
        centerx, 
        centery
    );
}

void UIRendering::renderrectangle(
    Rendering* r, UIRect rectangle, TextStyle style, bool filled)
{
    SDL_Color color = getcolorfromstyle(style);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    if(filled){
        r->renderfilledrectangle(uitoscreen(rectangle), false, false);
    }
    else{
        r->renderrectangle(uitoscreen(rectangle), false, false);
    }
}


void UIRendering::renderline(
    Rendering* r, UIVec start, UIVec end, TextStyle style)
{
    SDL_Color color = getcolorfromstyle(style);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    r->renderline(uitoscreen(start), uitoscreen(end), false);
}

SDL_Color UIRendering::getcolorfromstyle(UI::TextStyle style)
{
    SDL_Color color;
    switch (style)
    {
    case UI::Info:
        color = {0,0,0,255};
        break;
    case UI::Highlighted:
        color = {255,255,255,255};
        break;
    case UI::InvertedInfo:
        color = {255,255,255,255};
        break;
    case UI::InvertedHighlighted:
        color = {127,127,127,255};
        break;
    case UI::Inaccessible:
        color = {127,63,63,255};
        break;
    case UI::MapOverlay:
        color = {255,255,255,255};
        break;
    case UI::PanelBackground:
        color = {127,160,100,255};
        break;
    case UI::PanelBorder:
        color = {0,0,0,127};
        break;
    case UI::TooltipBackground:
        color = {0,0,0,63};
        break;
    case UI::TooltipBorder:
        color = {0,0,0,255};
        break;
    default:
        std::cout<<"Warning: UI style "<<style<<" not supported in InterfaceManager::rendertext"<<std::endl;
        break;
    }
    return color;
}

UIRect UIRendering::rendertext(
    Rendering* r, 
    std::string text, 
    UIRect rect, 
    TextStyle style, 
    bool centered, 
    Coord margin_x, 
    Coord margin_y)
{
    SDL_Color color = getcolorfromstyle(style);
    rect.x += margin_x;
    rect.w -= 2*margin_x;
    rect.y += margin_y;
    rect.h -= 2*margin_y;
    SDL_Rect absrect = uitoscreen(rect);
    SDL_Rect screentextrect;
    if(centered)
        screentextrect = r->rendercenteredtext(
            text, 
            absrect.x+absrect.w*0.5, 
            absrect.y+absrect.h*0.5, 
            color, false, false, absrect.w
        );
    else
        screentextrect = r->rendertext(
            text, absrect.x, absrect.y, color, false, false, absrect.w);
    UIRect textrect = screentoui(screentextrect);
    textrect.x -= margin_x;
    textrect.w += 2*margin_x;
    textrect.y -= margin_y;
    textrect.h += 2*margin_y;
    return textrect;
}

UIRect UIRendering::gettextsize(
    std::string text, UIRect maxrect, Coord margin_x, Coord margin_y)
{
    Coord maxtextuiwidth = std::max(maxrect.w - 2*margin_x, Coord{0});
    auto textsz = textsize(text, maxtextuiwidth*uiscale);
    Coord textuiwidth = textsz.first/uiscale;
    Coord textuiheight = textsz.second/uiscale;
    return UIRect{
        maxrect.x, 
        maxrect.y, 
        textuiwidth+2*margin_x, 
        textuiheight+2*margin_y
    };
}

std::string UIRendering::croptexttowidth(
    const std::string& text, Coord maxwidth, Coord margin_x)
{
    maxwidth -= 2*margin_x;
    int maxwidthint = round(maxwidth*uiscale);
    int ncharactersfitting;
    TTF_MeasureUTF8(font, text.c_str(), maxwidthint, NULL, &ncharactersfitting);

    if(ncharactersfitting>=text.size())
        return text;

    if(ncharactersfitting>3)
        return text.substr(0, ncharactersfitting-3)+"...";

    return text.substr(0, ncharactersfitting);
}

float UIRendering::getuiscale()
{
    return uiscale;
}

void UIRendering::increaseuiscale()
{
    setuiscale(uiscale + 0.2);
}

void UIRendering::decreaseuiscale()
{
    setuiscale(uiscale - 0.2);
}

void UIRendering::setuiscale(float newscale)
{
    auto oldscale = uiscale;
    uiscale = std::fmax(0.8, newscale);
    if(uiscale != oldscale) {
        int newfontsize = 12*uiscale;
        setfontsize(newfontsize);
        ui.handlewindowsizechange();
    }
}

SDL_Rect UIRendering::uitoscreen(UIRect uirect)
{
    float scale = getuiscale();
    // Computing the height and width as a difference ensures that (y+h)-y=h also for the
    // SDL_Rect
    SDL_Rect screenrect = {
        iround(uirect.x*scale),
        iround(uirect.y*scale),
        iround((uirect.x+uirect.w)*scale)-iround(uirect.x*scale),
        iround((uirect.y+uirect.h)*scale)-iround(uirect.y*scale)
    };
    return screenrect;
}

UIRect UIRendering::screentoui(SDL_Rect screenrect)
{
    float scale = getuiscale();
    // Computing the height and width as a difference should ensures that (y+h)-y=h also 
    // for the UIRect
    UIRect uirect = {
        screenrect.x/scale,
        screenrect.y/scale,
        (screenrect.x+screenrect.w)/scale - screenrect.x/scale,
        (screenrect.y+screenrect.h)/scale - screenrect.y/scale
    };
    return uirect;
}

Vec UIRendering::uitoscreen(UIVec uipos)
{
    float scale = getuiscale();
    Vec pos(uipos.x*scale, uipos.y*scale);
    return pos;
}

UIVec UIRendering::screentoui(Vec pos)
{
    float scale = getuiscale();
    UIVec uipos(pos.x/scale, pos.y/scale);
    return uipos;
}

UIRect UIRendering::getuiview()
{
    UIVec size = screentoui(getviewsize());
    return {0,0,size.x,size.y};
}

void UIRendering::renderscaleruler(
    Rendering* r, Coord leftx, Coord lefty, Coord scalelinelength)
{
    float uiscale = getuiscale();

    Coord textheight = 14;
    Coord markersize = 2;
	renderline(r,
        {leftx, lefty}, 
        {leftx+scalelinelength, lefty},
        UI::MapOverlay
    );
	renderline(r,
        {leftx, lefty-markersize}, 
        {leftx, lefty+markersize}, 
        UI::MapOverlay
    );
	renderline(r,
        {leftx+scalelinelength, lefty-markersize}, 
        {leftx+scalelinelength, lefty+markersize}, 
        UI::MapOverlay
    );
    std::string scaletext = std::to_string(
        iround(pixelstometers(scalelinelength) / r->getcamscale() * uiscale)
        ) + " m";
	rendertext(r, 
        scaletext, 
        {leftx, lefty-textheight, scalelinelength, textheight}, 
        UI::MapOverlay, 
        true
    );
}