#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "shares.h"

class Entity;

namespace Economy {

class Stock;
class Stake;
class Account;
class PlayerControl;

/* A list of stocks where this entity has stakes */
class Portfolio
{
public:
    Portfolio(Entity& e, Account& a, PlayerControl& controlled) : 
        entity{e}, account{a}, playercontrol(controlled) {}
    bool buy(Portfolio& fromportfolio, Stock& stock, Shares amount);
    bool buystake(Stake& fromstake, Account& payableaccount, Shares amount);
    const std::set<Stock*>& getstocks() {return stocks;}
    Entity& getentity() {return entity;}
    bool isplayercontrolled();
private:
    Entity& entity;
    Account& account;
    std::set<Stock*> stocks;
    PlayerControl& playercontrol;
};

} // namespace Economy