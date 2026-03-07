#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "bahnhof/economy/money.h"

class Entity;
class InterfaceManager;

namespace Economy{

class Account;

template<typename T>
class Control
{
public:
    Control(Entity& e, Account& a) : entity{e}, account{a} {}
    void listpossession(T& pos) {possessions.push_back(&pos);}
    bool delistpossession(T& pos) {return std::erase(possessions, &pos);}
    Account& getaccount() {return account;}
    Entity& getentity() {return entity;}
    void createpanel(InterfaceManager* ui); // Specializations defined in .cpp
private:
    Entity& entity;
    Account& account;
    std::vector<T*> possessions;
    UI::Ownership panel;
};

template<typename T>
bool buy(T& pos, Control<T>& buyer, Money price);

} // end namespace Economy