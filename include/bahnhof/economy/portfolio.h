#pragma once
#include "bahnhof/common/forwardincludes.h"

class Entity;

namespace Economy {
    
class Stock;
class Stake;
class Account;

/* A list of stocks where this entity has stakes */
class Portfolio
{
public:
    Portfolio(Entity& e, Account& a) : entity{e}, account{a} {}
    bool buy(Portfolio& fromportfolio, Stock& stock, uint16_t amount);
    bool buy(Stake& fromstake, Account& payableaccount, uint16_t amount);
    Account& getaccount() {return account;}
    const std::set<Stock*>& getstocks() {return stocks;}
    Entity& getentity() {return entity;}
private:
    Entity& entity;
    Account& account;
    std::set<Stock*> stocks;
};

} // namespace Economy