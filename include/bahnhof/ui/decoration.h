#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "bahnhof/common/math.h"
#include "bahnhof/ui/ui.h"

class Rendering;
class Train;
struct TrainInfo;
struct WagonInfo;

namespace UI{

class Text : public Element
{
public:
    Text(Host* p, std::string t, SDL_Rect r) : Element(p), text(t) {rect = r;};
    void render(Rendering*);
    std::string text;
    SDL_Color color = {0,0,0,255};
    bool centered = true;
    TextStyle style = Info;
};

class TrainIcons : public Element
{
public:
    TrainIcons(Host* p, SDL_Rect maxarea, Train& t) : Element(p), train(t) {rect = maxarea;};
    void render(Rendering*);
    void mousehover(Vec pos, int ms);
    void leftclick(Vec mousepos);
private:
    int getwagonidatmousepos(Vec mousepos);
    Train& train;
    std::vector<SDL_Rect> iconrects;
    int rendersplitafterwagonid = -1;
};

std::vector<SDL_Rect> rendertrainicons(Rendering* r, InterfaceManager& ui, std::vector<WagonInfo>& wagoninfos, SDL_Rect rect, int splitid=-1);

}
