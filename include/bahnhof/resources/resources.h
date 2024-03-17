#ifndef RESOURCES_H
#define RESOURCES_H

#include<map>
#include "bahnhof/common/math.h"

enum resourcetype
{
    none=-1, beer, hops, barley
};

class Rendering;

class Resource
{
public:
    Resource(resourcetype newtype, std::string newname, std::string pathtotex);
    void render(Rendering* r, Vec pos);
    resourcetype type;
private:
    std::string name;
    SDL_Texture* tex;
    int w;
    int h;
};

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();
    Resource* get(resourcetype type);
private:
    std::map<resourcetype, Resource*> resourcemap;
};

#endif