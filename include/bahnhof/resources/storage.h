#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<map>
#include<set>
#include "bahnhof/common/math.h"
#include "resourcetypes.h"

class Game;
class ResourceManager;
class Rendering;

class Storage
{
public:
    Storage(Game* game, int x, int y, int w, int h);
    void render(Rendering* r);
    bool containspoint(Vec pos);
    int loadstorage(resourcetype type, int amount);
    int unloadstorage(resourcetype type, int amount);
    bool accepts(resourcetype resource);
    bool provides(resourcetype resource);
    void setaccepting(resourcetype resource, bool shouldaccept);
    void setproviding(resourcetype resource, bool shouldprovide);
    resourcetype getfirstprovidedresource();
private:
    std::set<resourcetype> providedresources;
    std::set<resourcetype> acceptedresources;
    Game* game;
    ResourceManager* allresources;
    std::map<resourcetype, int> storedresources;
    SDL_Rect rect;
};

Storage* getstorageatpoint(Vec pos);

extern std::vector<std::unique_ptr<Storage> > storages;