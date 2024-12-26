#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<vector>
#include "bahnhof/common/math.h"
#include "bahnhof/rollingstock/train.h"

class Game;
class Gamestate;
class Rendering;
class InterfaceManager;

namespace UI{
class Panel;
class Table;

class Element
{
public:
    Element(Panel*);
    virtual ~Element() {};
    virtual void update(int ms) {};
    virtual void render(Rendering*) {};
    SDL_Rect getglobalrect();
    SDL_Rect getlocalrect();
protected:
    Panel* panel;
    SDL_Rect rect = {0,0,100,100};
    InterfaceManager* ui;
    Game* game;
};

class TableLine : public Element
{
public:
    TableLine(Panel*, Table*);
    virtual void render(Rendering* r, SDL_Rect maxarea) {std::cout<<"but this does"<<std::endl;};
private:
    Table* table;
};

class TableTextLine : public TableLine
{
public:
    TableTextLine(Panel*, Table*, std::string newstr);
    virtual void render(Rendering* r, SDL_Rect maxarea);
private:
    std::string str;
};

class Table : public Element
{
public:
    Table(Panel*, SDL_Rect newrect);
    virtual ~Table() {};
    virtual void render(Rendering*);
protected:
    std::vector<std::unique_ptr<TableLine>> lines;
};

class MainInfoTable : public Table
{
public:
    MainInfoTable(Panel*, SDL_Rect newrect);
    void update(int ms);
};

class RoutesTable : public Table
{
public:
    RoutesTable(Panel* newpanel, SDL_Rect newrect) : Table(newpanel, newrect) {};
    void update(int ms);
};

class TrainTable : public Table
{
public:
    TrainTable(Panel*, SDL_Rect newrect);
    void update(int ms);
protected:
    TrainManager* trainmanager;
    std::vector<TrainInfo> traininfos;
	std::vector<std::string> trainstrings;
	std::vector<float> trainspeeds;
};

}