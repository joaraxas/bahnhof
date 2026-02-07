#include "bahnhof/ui/ui.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/input/input.h"

using namespace UI;

Tooltip::Tooltip(InputManager& i, UIRendering& u)
    : input(i), uirendering(u) {}

void Tooltip::set(std::string tip)
{
    tips += tip + '\n';
}

void Tooltip::render(Rendering* r)
{
    Vec mousepos = input.screenmousepos();
    SDL_Rect textrect(mousepos.x+50, mousepos.y, 200, 180);
    auto uirect = uirendering.screentoui(textrect);
    uirendering.rendertext(r, tips, uirect, UI::MapOverlay);

    tips.clear();
}