#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "bahnhof/ui/panels.h"

class Company;

class Stake
{
public:
    Stake() : amount(0) {};
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
    uint16_t addamount(uint16_t howmany) {return amount+=howmany;}
    bool setcompany(Company& c) {
        if(!company){
            company = &c;
            return true;
        }
        return false;
    }
private:
    uint16_t amount;
    Company* company = nullptr;
};
