#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/economy/person.h"
#include "bahnhof/economy/thepublic.h"
#include "bahnhof/economy/stockmarket.h"
#include "bahnhof/input/controlmode.h"

class Game;
class Rendering;
class InputManager;
class TimeManager;

namespace Economy{
    class Company;
}

class EconomyManager
{
public:
    EconomyManager(Game* whatgame);
    void update(int ms);
    Economy::Company& getmycompany() {if(companies.empty()) std::cout<<"no company"<<std::endl; return *companies.front().get();};
    Economy::Person me;
    Economy::ThePublic thepublic;
    Economy::Stockmarket stockmarket;
private:
    Game* game;
    std::vector<std::unique_ptr<Economy::Company>> companies;
};
