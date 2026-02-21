#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "bahnhof/ui/panels.h"

class Company;

class Stake
{
public:
    Stake(Company& c) : amount(0), company(&c) {}
    Stake(Company& c, uint16_t a) : amount(a), company(&c) {}
    bool buyfrom(Stake& from, int howmany) {
        if(from.company!=company) return false;
        if(from.amount>=howmany){
            from.amount -= howmany;
            amount += howmany;
            return true;
        }
        return false;
    }
    uint16_t getamount() const {return amount;}
    Company& getcompany() const {return *company;}
private:
    uint16_t addamount(uint16_t howmany) {return amount+=howmany;}
    uint16_t amount;
    Company* company;
};
