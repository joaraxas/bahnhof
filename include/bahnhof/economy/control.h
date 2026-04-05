#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "bahnhof/economy/money.h"

class Entity;
class InterfaceManager;

namespace Economy{

class Account;
class PlayerControl;

/* This class represents a list of all its entity's possessions of type T */
template<typename T>
class Control
{
public:
    Control(Entity& e, Account& a, PlayerControl& c) : 
        entity{e}, account{a}, playercontrol{c} {}
    void listpossession(T& pos) {possessions.push_back(&pos);}
    bool delistpossession(T& pos) {return std::erase(possessions, &pos);}
    Account& getaccount() {return account;}
    Entity& getentity() {return entity;}
    PlayerControl& getplayercontrol() {return playercontrol;}
    void createpanel(InterfaceManager* ui); // Specializations defined in .cpp
private:
    PlayerControl& playercontrol;
    Entity& entity;
    Account& account;
    std::vector<T*> possessions;
    UI::Ownership panel;
};

template<typename T>
bool buy(T& possession, Control<T>& buyer, Money price);

} // end namespace Economy