#pragma once
#include "bahnhof/graphics/sprite.h"
#include "bahnhof/common/math.h"
#include "bahnhof/resources/resourcetypes.h"


class Game;
class Rendering;
class Shape;

class Resource
{
public:
    Resource(SpriteManager& s,
        resourcetype newtype,
        std::string newname,
        sprites::name iconname,
        sprites::name texturename);
    void render(Rendering* r, Vec pos);
    void renderasshape(Rendering* r, const Shape* shape);
    resourcetype type;
private:
    std::string name;
    Icon icon;
    Sprite fillingtexture;
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
