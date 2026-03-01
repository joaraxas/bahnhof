#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "money.h"
#include "stake.h"

class Portfolio;
class Account;
class Entity;

class Stock
{
public:
    Stock(Entity& e, Account& a) : entity{e}, account{a} {}
    bool issue(Money investment, Portfolio& buyer);
    Money getvaluation() const {return valuation;}
    Money getshareprice() const {
        if(shares == 0) return 5;
        return valuation/shares;
    }
    uint16_t getnumshares() const {return shares;}
    Stake* const getstakeforportfolio(Portfolio& who);
    Stake& registernewstake(Portfolio& who);
    bool removeemptystake(Portfolio& who);
    void updateregistry();
    const std::vector<std::pair<Stake*, Entity*>>& getsortedowners() 
        {return sortedowners;}
    Entity& getentity() {return entity;}
private:
    Entity& entity;
    Account& account;
    uint16_t shares{0};
    std::unordered_map<Portfolio*, Stake> stakes;
    std::vector<std::pair<Stake*, Entity*>> sortedowners;
    Money valuation{0};
};

