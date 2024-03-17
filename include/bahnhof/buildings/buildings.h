#pragma once
#include "bahnhof/resources/resources.h"

class Building
{
public:
    Building(ResourceManager* resources, int x, int y, int w, int h, resourcetype need, resourcetype production);
    virtual void render(Rendering* rendering);
    void update(int ms);
protected:
    SDL_Color color;
private:
    Storage* storage;
    SDL_Rect rect;
    int timeleft = 3000;
    ResourceManager* allresources;
    resourcetype wants;
    resourcetype makes;
};

class Brewery : public Building
{
public:
    Brewery(ResourceManager* resources, Vec pos);
};

class Hopsfield : public Building
{
public:
    Hopsfield(ResourceManager* resources, Vec pos);
};

class City : public Building
{
public:
    City(ResourceManager* resources, Vec pos);
};

