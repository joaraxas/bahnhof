#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/economy/person.h"
#include "bahnhof/economy/thepublic.h"
#include "bahnhof/economy/stockmarket.h"
#include "bahnhof/input/controlmode.h"

class Game;

namespace Economy{
    class Company;
}

class EconomyManager
{
public:
    EconomyManager(Game* whatgame);
    void update(int ms);
    Economy::ThePublic thepublic;
    Economy::Stockmarket stockmarket;
private:
    Game* game;
    std::vector<std::unique_ptr<Economy::Company>> companies;
    std::vector<std::unique_ptr<Economy::Person>> persons;
};
