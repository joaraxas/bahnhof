#include<iostream>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/decoration.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/rollingstock/train.h"

namespace UI{

void Text::render(Rendering* r)
{
    SDL_Rect screenrect = ui->uitoscreen(getglobalrect());
    if(centered)
        r->rendercenteredtext(text, int(screenrect.x+screenrect.w*0.5), int(screenrect.y+screenrect.h*0.5), color, false, false, screenrect.w);
    else
        r->rendertext(text, screenrect.x, screenrect.y, color, false, false, screenrect.w);
}

void TrainIcons::render(Rendering* r)
{
    rendertrainicons(r, *ui, train.getinfo(), getglobalrect());
}

}