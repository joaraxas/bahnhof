#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "bahnhof/common/math.h"

class Game;
class Rendering;
class InterfaceManager;

namespace UI{

class Host;
class Dropdown;

enum TextStyle{
    Info,
    Highlighted,
    InvertedInfo,
    MapOverlay
};

class Element
{
public:
    Element(Host*);
    virtual ~Element() {};
    virtual bool checkclick(Vec pos);
    virtual void mousehover(Vec pos, int ms) {};
    virtual void leftclick(Vec pos) {};
    virtual void scroll(Vec pos, int distance) {};
    virtual void leftpressed(Vec pos, int mslogic) {};
    virtual void update(int ms) {};
    virtual void render(Rendering*) {};
    virtual SDL_Rect getglobalrect();
    SDL_Rect getlocalrect();
protected:
    Host* panel;
    SDL_Rect rect = {0,0,100,100};
    InterfaceManager* ui;
    Game* game;
};

class Host
{
public:
    Host(InterfaceManager* newui, SDL_Rect newrect);
    virtual ~Host() {};
    bool checkclick(Vec pos);
    void mousehover(Vec pos, int ms);
    void click(Vec pos, int type);
    void scroll(Vec pos, int distance);
    void mousepress(Vec pos, int mslogic, int type);
    virtual void update(int ms);
    virtual void render(Rendering*);
    void addelement(Element*);
    void move(Vec towhattopcorner);
    virtual void erase();
    InterfaceManager& getui();
    SDL_Rect getglobalrect();
    SDL_Rect getlocalrect();
protected:
    Element* getelementat(Vec pos);
    Game* game;
    SDL_Rect rect;
    std::vector<std::unique_ptr<Element>> elements;
    InterfaceManager* ui;
};

}

class UIRendering{
public:
    UIRendering(InterfaceManager& newui);
    void rendertexture(Rendering* r, SDL_Texture* tex, SDL_Rect* rect, SDL_Rect* srcrect=nullptr, float angle=0, bool ported=false, bool originiscenter=false, int centerx=0, int centery=0);
    void renderrectangle(Rendering* r, SDL_Rect rectangle, UI::TextStyle style, bool filled=false);
    SDL_Rect rendertext(Rendering*, std::string, SDL_Rect, UI::TextStyle, bool centered=false, int margin_x=0, int margin_y=0);
    SDL_Rect gettextsize(std::string text, SDL_Rect maxrect, int margin_x=0, int margin_y=0);
    std::string croptexttowidth(const std::string& text, int maxwidth, int margin_x=0);
    float getuiscale();
    void increaseuiscale();
    void decreaseuiscale();
    SDL_Rect uitoscreen(SDL_Rect rect);
    SDL_Rect screentoui(SDL_Rect rect);
    Vec uitoscreen(Vec pos);
    Vec screentoui(Vec pos);
    void renderscaleruler(Rendering* r, int leftx, int lefty, int scalelinelength);
private:
    float uiscale = 1;
    void setuiscale(float newscale);
    SDL_Color getcolorfromstyle(UI::TextStyle style);
    InterfaceManager& ui;
};

class InterfaceManager{
public:
    InterfaceManager(Game*);
    void update(int ms);
    bool mousehover(Vec pos, int ms);
    bool click(Vec pos, int type);
    bool scroll(Vec pos, int distance);
    void leftbuttonup(Vec pos);
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
    UI::Host* getpanelat(Vec pos);
    void cleanup();
    std::vector<std::unique_ptr<UI::Host>> panels;
    std::vector<UI::Host*> panelstodelete;
    UI::Dropdown* dropdown = nullptr;
    Vec movingwindowoffset;
    Game* game;
    UIRendering uirendering;
};