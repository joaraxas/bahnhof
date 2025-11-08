#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/common/math.h"
#include "bahnhof/common/orientation.h"
#include "bahnhof/ui/uimath.h"
#include "bahnhof/ui/uistyle.h"

class Game;
class Rendering;
class InterfaceManager;

namespace UI{

class Host;
class Dropdown;

}

class UIRendering{
    using UIRect = UI::UIRect;
    using UIVec = UI::UIVec;
    using Coord = UI::Coord;
    using TextStyle = UI::TextStyle;
public:
    UIRendering(InterfaceManager& newui);
    void rendertexture(
        Rendering* r, 
        SDL_Texture* tex, 
        UIRect rect, 
        SDL_Rect* srcrect=nullptr, 
        Angle angle=Angle::zero, 
        bool ported=false, 
        bool originiscenter=false, 
        int centerx=0, 
        int centery=0);
    void renderrectangle(
        Rendering* r, 
        UIRect rectangle, 
        TextStyle style, 
        bool filled=false);
    UIRect rendertext(
        Rendering*, 
        std::string, 
        UIRect, 
        TextStyle, 
        bool centered=false, 
        Coord margin_x=0, 
        Coord margin_y=0);
    UIRect gettextsize(
        std::string text, 
        UIRect maxrect, 
        Coord margin_x=0, 
        Coord margin_y=0);
    std::string croptexttowidth(
        const std::string& text, 
        Coord maxwidth, 
        Coord margin_x=0);
    float getuiscale();
    void increaseuiscale();
    void decreaseuiscale();
    SDL_Rect uitoscreen(UIRect rect);
    UIRect screentoui(SDL_Rect rect);
    Vec uitoscreen(UIVec pos);
    UIVec screentoui(Vec pos);
    void renderscaleruler(
        Rendering* r, Coord leftx, Coord lefty, Coord scalelinelength);
private:
    float uiscale = 1;
    void setuiscale(float newscale);
    SDL_Color getcolorfromstyle(TextStyle style);
    InterfaceManager& ui;
};

class InterfaceManager{
public:
    InterfaceManager(Game*);
    ~InterfaceManager();
    void update(int ms);
    bool mousehover(Vec pos, int ms);
    bool click(Vec pos, int type);
    bool scroll(Vec pos, int distance);
    bool leftbuttonup(Vec pos);
    bool leftpressed(Vec pos, int mslogic);
    void render(Rendering*);
    
    void addpanel(UI::Host*);
    void removepanel(UI::Host*);
    void movepaneltofront(UI::Host*);
    void setdropdown(UI::Dropdown*);
    Game& getgame();
    UIRendering& getuirendering() {return uirendering;};
    UI::Host* movingwindow = nullptr;
private:
    UI::Host* getpanelat(UI::UIVec pos);
    void cleanup();
    std::vector<std::unique_ptr<UI::Host>> panels;
    std::vector<UI::Host*> panelstodelete;
    UI::Dropdown* dropdown = nullptr;
    UI::UIVec movingwindowoffset;
    Game* game;
    UIRendering uirendering;
};