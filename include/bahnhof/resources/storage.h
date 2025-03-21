#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<map>
#include "bahnhof/common/math.h"
#include "resourcetypes.h"

class Game;
class ResourceManager;
class Rendering;

class Storage
{
public:
    Storage(Game* game, int x, int y, int w, int h, resourcetype _accepts, resourcetype _provides);
    void render(Rendering* r);
    int loadstorage(resourcetype type, int amount);
    int unloadstorage(resourcetype type, int amount);
    bool containspoint(Vec pos);
    resourcetype getfirststoredresource();
    resourcetype accepts;
    resourcetype provides;
private:
    Game* game;
    ResourceManager* allresources;
    std::map<resourcetype, int> storedresources;
    SDL_Rect rect;
};

Storage* getstorageatpoint(Vec pos);

extern std::vector<std::unique_ptr<Storage> > storages;