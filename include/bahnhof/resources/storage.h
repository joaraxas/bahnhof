#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<map>
#include<set>
#include "bahnhof/common/math.h"
#include "resourcetypes.h"
#include "bahnhof/economy/control.h"
#include "bahnhof/economy/playercontrol.h"

class Game;
class ResourceManager;
class Rendering;
class Storage;

using StorageOwner = Economy::Control<Storage>;

class Storage
{
public:
    Storage(Game* game, StorageOwner& o, int x, int y, int w, int h);
    ~Storage();
    void render(Rendering* r);
    bool containspoint(Vec pos);
    int loadstorage(resourcetype type, int amount);
    int unloadstorage(resourcetype type, int amount);
    bool accepts(resourcetype resource);
    bool provides(resourcetype resource);
    void setaccepting(resourcetype resource, bool shouldaccept);
    void setproviding(resourcetype resource, bool shouldprovide);
    resourcetype getfirstprovidedresource();
    StorageOwner& getowner() const {return control.getowner();};
    void setowner(StorageOwner& newowner) {control.setowner(newowner);};
private:
    std::set<resourcetype> providedresources;
    std::set<resourcetype> acceptedresources;
    Game* game;
    ResourceManager* allresources;
    std::map<resourcetype, int> storedresources;
    SDL_Rect rect;
    Economy::ControllerPointer<StorageOwner> control;
};

Storage* getstorageatpoint(Vec pos);

extern std::vector<std::unique_ptr<Storage> > storages;