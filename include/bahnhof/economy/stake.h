#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "bahnhof/ui/panels.h"

class Company;

class Stake
{
public:
    Stake() : amount(0) {}
    Stake(Company& c) : amount(0) {setcompany(c);}
    Stake(Company& c, uint16_t a) : amount(a) {setcompany(c);}
    ~Stake();
    bool operator<(const Stake& other) {return company<other.company;}
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
    bool setcompany(Company& c);
    Company* getcompany() const {return company;}
private:
    uint16_t amount;
    Company* company = nullptr;
};
