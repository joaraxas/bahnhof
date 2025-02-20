#ifndef RESOURCES_H
#define RESOURCES_H

#include<map>
#include "bahnhof/graphics/sprite.h"
#include "bahnhof/common/math.h"

enum resourcetype
{
    none=-1, beer, hops, barley
};

class Game;
class Rendering;

class Resource
{
public:
    Resource(SpriteManager& s, resourcetype newtype, std::string newname, sprites::name spritename);
    void render(Rendering* r, Vec pos);
    resourcetype type;
private:
    std::string name;
    Icon icon;
    Game* game;
};

class ResourceManager
{
public:
    ResourceManager(Game* whatgame);
    ~ResourceManager();
    Resource* get(resourcetype type);
    Game& getgame() {return *game;};
private:
    std::map<resourcetype, Resource*> resourcemap;
    Game* game;
};

#endif