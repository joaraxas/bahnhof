#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "bahnhof/ui/panels.h"

class Entity;
class Account;
class InterfaceManager;

namespace Economy{

template<typename T>
class Control
{
public:
    Control(Entity& e, Account& a) : entity{e}, account{a} {}
    void listpossession(T& pos) {possessions.push_back(&pos);}
    bool delistpossession(T& pos) {return std::erase(possessions, &pos);}
    Account& getaccount() {return account;}
    Entity& getentity() {return entity;}
    void createpanel(InterfaceManager* ui) {
        if(!panel.exists())
            panel.set(
                new UI::EconomyPanels::PossessionsPanel(ui, possessions)
            );
        else
            panel.movetofront();
    }
private:
    Entity& entity;
    Account& account;
    std::vector<T*> possessions;
    UI::Ownership panel;
};

} // end namespace Economy