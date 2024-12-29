#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<vector>
#include "bahnhof/ui/buttons.h"
#include "bahnhof/common/math.h"
#include "bahnhof/rollingstock/train.h"

class Game;
class Gamestate;
class Rendering;
class InterfaceManager;

namespace UI{
class Panel;
class Table;

class TableLine : virtual public Element
{
public:
    TableLine(Panel*, Table*);
    virtual void render(Rendering* r, SDL_Rect maxarea) {std::cout<<"but this does"<<std::endl;};
    SDL_Rect getglobalrect();
protected:
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

class TrainTableLine : public TableLine, public Button
{
public:
    TrainTableLine(Panel*, Table*, TrainInfo);
    void render(Rendering* r, SDL_Rect maxarea);
private:
    void click();
    TrainInfo info;
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
};

}