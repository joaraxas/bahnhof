#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "money.h"
#include "stake.h"

class Entity;

namespace Economy {

class Portfolio;
class Account;
class Stockmarket;
class PlayerControl;

/* A shared ownership of this entity, i.e. a collection of shares along 
with a valuation */
class Stock
{
public:
    Stock(Entity& e, Account& a, Stockmarket& sm);
    ~Stock();
    void update(int ms);
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
    bool attempttakeover();
private:
    Entity& entity;
    Account& account;
    Stockmarket& market;
    uint16_t shares{0};
    std::unordered_map<Portfolio*, Stake> stakes;
    std::vector<std::pair<Stake*, Entity*>> sortedowners;
    Money valuation{0};
    Money lastprofit{0};
};

} // namespace Economy