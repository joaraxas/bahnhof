#include "bahnhof/input/builder.h"
#include "bahnhof/input/controlmode.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/resources/storage.h"
#include "bahnhof/economy/money.h"


StorageBuilder::StorageBuilder(InputManager& owner, Game* newgame) : Builder(owner, newgame)
{
}

void StorageBuilder::render(Rendering* r)
{
    Builder::render(r);
    if(canbuild()){
        SDL_SetRenderDrawColor(renderer,0,127,0,255);
    }
    else{
        SDL_SetRenderDrawColor(renderer,127,0,0,255);
    }
    r->renderrectangle(getspace());
}

bool StorageBuilder::canfit()
{
    StorageOwner* contractor = game->getcontrolmode().storages;
    if(!contractor)
        return false; // not about "fitting" but needs to be checked here.
    cost = 2_Fr;
    return true;
}

void StorageBuilder::build()
{
    StorageOwner& contractor = *game->getcontrolmode().storages;
    SDL_Rect s = getspace();
    storages.emplace_back(std::make_unique<Storage>(game, contractor, s.x, s.y, s.w, s.h));
}

SDL_Rect StorageBuilder::getspace() const
{
    Vec corner1 = orientor.getanchor();
    return {static_cast<int>(corner1.x), static_cast<int>(corner1.y), 700, 700};
}