#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "bahnhof/ui/panels.h"

class Company;

class Stake
{
public:
    Stake(int howmany) : amount(howmany) {};
    bool buyfrom(Stake& from, int howmany) {
        if(from.company!=company) return false;
        if(from.amount>=howmany){
            from.amount -= howmany;
            amount += howmany;
            return true;
        }
        return false;
    }
    int getamount() {return amount;}
    bool setcompany(Company& c) {
        if(!company){
            company = &c;
            return true;
        }
        return false;
    }
private:
    int amount;
    Company* company;
};
