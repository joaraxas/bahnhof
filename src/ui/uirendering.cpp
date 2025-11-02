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
    UIRect* rect, 
    SDL_Rect* srcrect, 
    Angle angle, 
    bool ported, 
    bool originiscenter, 
    int centerx, 
    int centery)
{
    SDL_Rect screenrect = uitoscreen(*rect);
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
    case UI::MapOverlay:
        color = {255,255,255,255};
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
    Coord maxtextuiwidth = maxrect.w - 2*margin_x;
    SDL_Texture* tex = loadtext(
        text, {0,0,0,255}, maxtextuiwidth*uiscale);
    int textscreenwidth, textscreenheight;
	SDL_QueryTexture(
        tex, NULL, NULL, &textscreenwidth, &textscreenheight);
	SDL_DestroyTexture(tex);
    Coord textuiwidth = textscreenwidth/uiscale;
    Coord textuiheight = textscreenheight/uiscale;
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
    TTF_MeasureText(font, text.c_str(), maxwidthint, NULL, &ncharactersfitting);

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
    uiscale = std::fmax(0.8, newscale);;
    int newfontsize = 12*uiscale;
    setfontsize(newfontsize);
}

SDL_Rect UIRendering::uitoscreen(UIRect uirect)
{
    float scale = getuiscale();
    SDL_Rect screenrect = {
        int(round(uirect.x*scale)), 
        int(round(uirect.y*scale)), 
        int(round(uirect.w*scale)), 
        int(round(uirect.h*scale))
    };
    return screenrect;
}

UIRect UIRendering::screentoui(SDL_Rect screenrect)
{
    float scale = getuiscale();
    UIRect uirect = {
        int(round(screenrect.x/scale)), 
        int(round(screenrect.y/scale)), 
        int(round(screenrect.w/scale)), 
        int(round(screenrect.h/scale))
    };
    return uirect;
}

Vec UIRendering::uitoscreen(UIVec uipos)
{
    float scale = getuiscale();
    Vec pos = Vec(uipos.x*scale, uipos.y*scale);
    return pos;
}

UIVec UIRendering::screentoui(Vec pos)
{
    float scale = getuiscale();
    UIVec uipos = Vec(pos.x/scale, pos.y/scale);
    return uipos;
}

void UIRendering::renderscaleruler(
    Rendering* r, Coord leftx, Coord lefty, Coord scalelinelength)
{
    float uiscale = getuiscale();

    int textheight = 14;
    SDL_Color c = getcolorfromstyle(UI::MapOverlay);
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    int markersize = 2;
	r->renderline(
        uitoscreen(UIVec{leftx,lefty}), 
        uitoscreen(UIVec{leftx+scalelinelength,lefty}), 
        false
    );
	r->renderline(
        uitoscreen(UIVec(leftx,lefty-markersize)), 
        uitoscreen(UIVec(leftx,lefty+markersize)), 
        false
    );
	r->renderline(
        uitoscreen(UIVec(leftx+scalelinelength,lefty-markersize)), 
        uitoscreen(UIVec(leftx+scalelinelength,lefty+markersize)), 
        false
    );
    std::string scaletext = std::to_string(
        int(round(scalelinelength*0.001*150/r->getcamscale()*uiscale))
        ) + " m";
	rendertext(r, 
        scaletext, 
        {leftx, lefty-textheight, scalelinelength, textheight}, 
        UI::MapOverlay, 
        true
    );
}