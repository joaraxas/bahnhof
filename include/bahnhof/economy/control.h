#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"

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
inline bool transferpossession(T& pos, Control<T>& to) {
    auto& owner = pos.getowner();
    if(&owner == &to) {
        std::cout<<owner.getentity().getname()<<
            " tried to buy "<<pos.getname()<<" from himself"<<std::endl;
        return false;
    }
    owner.delistpossession(pos); to.listpossession(pos); pos.setowner(to);
};

} // end namespace Economy