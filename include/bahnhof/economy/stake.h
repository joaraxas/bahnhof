#pragma once
#include "bahnhof/common/forwardincludes.h"

namespace Economy {

class Stock;

/* Ownership of a number of shares in a stock */
class Stake
{
public:
    Stake(Stock& c) : amount(0), stock(&c) {}
    Stake(Stock& c, uint16_t a) : amount(a), stock(&c) {}
    bool takefrom(Stake& from, int howmany);
    uint16_t getamount() const {return amount;}
    Stock& getstock() const {return *stock;}

    bool operator<(const Stake& other) {return amount<other.amount;}
    bool operator>(const Stake& other) {return amount>other.amount;}
private:
    uint16_t addamount(uint16_t howmany) {return amount+=howmany;}
    uint16_t amount;
    Stock* stock;
};

} // namespace Economy