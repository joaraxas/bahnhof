#pragma once
#include "bahnhof/common/forwardincludes.h"

class Stock;

class Stake
{
public:
    Stake(Stock& c) : amount(0), stock(&c) {}
    Stake(Stock& c, uint16_t a) : amount(a), stock(&c) {}
    bool buyfrom(Stake& from, int howmany) {
        if(from.stock!=stock) return false;
        if(from.amount>=howmany){
            from.amount -= howmany;
            amount += howmany;
            return true;
        }
        return false;
    }
    uint16_t getamount() const {return amount;}
    Stock& getstock() const {return *stock;}
private:
    uint16_t addamount(uint16_t howmany) {return amount+=howmany;}
    uint16_t amount;
    Stock* stock;
};
