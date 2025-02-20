#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<string>
#include "bahnhof/common/math.h"

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 600;
const int MAP_WIDTH = SCREEN_WIDTH*64;
const int MAP_HEIGHT = SCREEN_HEIGHT*64;

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;

extern bool nicetracks;

class Game; class Camera; class Gamestate;

class Rendering
{
public:
    Rendering(Game* whatgame, Camera* whatcam);
    void render(Gamestate* gamestate);
    SDL_Rect rendertext(std::string text, int x, int y, SDL_Color color={0,0,0,255}, bool ported=true, bool zoomed=false, int maxwidth=0);
    SDL_Rect rendercenteredtext(std::string text, int x, int y, SDL_Color color={0,0,0,255}, bool ported=true, bool zoomed=false, int maxwidth=0);
    void rendertexture(SDL_Texture* tex, SDL_Rect* rect, SDL_Rect* srcrect=nullptr, float angle=0, bool ported=true, bool zoomed=true, bool originiscenter=true, int centerx=0, int centery=0);
    void renderline(Vec pos1, Vec pos2, bool ported=true);
    void renderrectangle(SDL_Rect rect, bool ported=true, bool zoomed=true);
    void renderfilledrectangle(SDL_Rect rect, bool ported=true, bool zoomed=true);
    Vec getviewsize();
    float getcamscale();
    int getlogicalscale();
    Game& getgame() {return *game;};
private:
    Game* game;
    Camera* cam;
    SDL_Texture* fieldtex;
};

Vec randpos(int xoffset=0, int yoffset=0);