#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "money.h"
#include "stake.h"
#include "shares.h"

class Entity;

namespace Economy {

class Portfolio;
class Account;
class Stockmarket;

/* A shared ownership of this entity, i.e. a collection of shares along 
with a valuation */
class Stock
{
public:
    Stock(Entity& e, Account& a, Stockmarket& sm, 
        std::vector<std::pair<Portfolio*,Money>> owners={});
    ~Stock();
    void update(int ms);
    bool issue(Shares issuedshares, Portfolio& buyer);
    Money getvaluation() const {return valuation;}
    Money getshareprice() const {
        if(shares == 0) return 5;
        return valuation/shares;
    }
    Shares getnumshares() const {return shares;}
    Stake* const getstakeforportfolio(Portfolio& who);
    Stake& registernewstake(Portfolio& who);
    bool removeemptystake(Portfolio& who);
    void updateregistry();
    const std::vector<std::pair<Stake*, Entity*>>& getsortedowners() 
        {return sortedowners;}
    Entity& getentity() {return entity;}
    bool attempttakeover();
private:
    bool issue(Shares issuedshares, Portfolio& buyer, double devaluation);
    Entity& entity;
    Account& account;
    Stockmarket& market;
    Shares shares{0};
    std::unordered_map<Portfolio*, Stake> stakes;
    std::vector<std::pair<Stake*, Entity*>> sortedowners;
    Money valuation{0};
    Money lastrevenue{0};
    Money lastprofit{0};
};

} // namespace Economy