#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/economy/company.h"
#include "bahnhof/economy/stake.h"
#include "bahnhof/economy/owner.h"
#include "bahnhof/economy/money.h"

Stake::~Stake() {
    if(company){
        company->removestake(*this);
    }
}

bool Stake::setcompany(Company& c) {
    if(!company){
        company = &c;
        c.addstake(*this);
        return true;
    }
    return false;
}

bool Owner::buy(Owner& from, Company& company, uint16_t amount) {
    if(amount<=0)
        return false;
    Stake* hisstake{from.getstakeforcompany(company)};
    if(!hisstake)
        return false;
    if(!buy(*hisstake, from.account, amount))
        return false;
    if(hisstake->getamount() == 0)
        from.stakes.erase(&company);
}

bool Owner::buy(Stake& fromstake, Account& intoaccount, uint16_t amount) {
    if(fromstake.getamount()<amount)
        return false;
    Company* company = fromstake.getcompany();
    if(!company)
        return false;
    Money purchaseamount = amount * fromstake.getcompany()->getshareprice();
    if(!account.canafford(purchaseamount))
        return false;
    account.pay(purchaseamount, &intoaccount);
    Stake* mystake = getstakeforcompany(*company);
    if(!mystake){
        stakes.emplace(company, Stake(*company));
        mystake = &stakes[company];
    }
    mystake->buyfrom(fromstake, amount);
    return true;
}

Stake* Owner::getstakeforcompany(Company& company) {
    if(stakes.contains(&company))
        return &stakes.at(&company);
    return nullptr;
}

bool Company::emission(Money investment, Owner& buyer) {
    uint16_t emittedshares = std::lround(
        std::floor(investment/getshareprice()));
    if(emittedshares==0)
        return false;
    Stake newshares(*this, emittedshares);
    if(!buyer.buy(newshares, getaccount(), emittedshares))
        return false;
    return true;
}

bool Company::addstake(Stake& stake) {
    if(stake.getcompany() != this)
        return false;
    stakesincompany.insert(&stake);
    valuation += stake.getamount() * getshareprice();
    shares += stake.getamount();
    return true;
}

bool Company::removestake(Stake& stake) {
    if(stake.getcompany() != this)
        return false;
    stakesincompany.erase(&stake);
    valuation -= stake.getamount() * getshareprice();
    shares -= stake.getamount();
    return true;
}