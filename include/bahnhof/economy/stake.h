#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "shares.h"

namespace Economy {

class Stock;

/* Ownership of a number of shares in a stock */
class Stake
{
public:
    Stake(Stock& c) : amount(0), stock(&c) {}
    Stake(Stock& c, Shares a) : amount(a), stock(&c) {}
    bool takefrom(Stake& from, int howmany);
    Shares getamount() const {return amount;}
    Stock& getstock() const {return *stock;}

    bool operator<(const Stake& other) {return amount<other.amount;}
    bool operator>(const Stake& other) {return amount>other.amount;}
private:
    Shares addamount(Shares howmany) {return amount+=howmany;}
    Shares amount;
    Stock* stock;
};

} // namespace Economy