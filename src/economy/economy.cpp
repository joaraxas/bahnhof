#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/economy/company.h"
#include "bahnhof/economy/stake.h"
#include "bahnhof/economy/owner.h"
#include "bahnhof/economy/money.h"


bool Owner::buy(Owner& from, Company& company, uint16_t amount) {
    if(amount<=0)
        return false;
    Stake* hisstake{from.getstakeforcompany(company)};
    if(!hisstake)
        return false;
    if(!buy(*hisstake, from.account, amount))
        return false;
    if(hisstake->getamount() == 0){
        company.removeemptystake(*this);
        from.stakes.erase(&company);
    }
    return true;
}

bool Owner::buy(Stake& fromstake, Account& intoaccount, uint16_t amount) {
    if(fromstake.getamount()<amount)
        return false;
    Company& company = fromstake.getcompany();
    Money purchaseamount = amount * fromstake.getcompany().getshareprice();
    if(!account.canafford(purchaseamount))
        return false;
    account.pay(purchaseamount, &intoaccount);
    Stake* mystake = getstakeforcompany(company);
    if(!mystake){
        stakes.emplace(&company, company.registernewstake(*this));
        mystake = stakes.at(&company);
    }
    mystake->buyfrom(fromstake, amount);
    return true;
}

Stake* Owner::getstakeforcompany(Company& company) {
    if(stakes.contains(&company))
        return stakes.at(&company);
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
    valuation += getshareprice() * emittedshares;
    shares += emittedshares;
    return true;
}

Stake* Company::registernewstake(Owner& who) {
    auto result = stakesincompany.emplace(&who, Stake(*this));
    return &result.first->second;
}

bool Company::removeemptystake(Owner& who) {
    if(!stakesincompany.contains(&who))
        return true;
    if(stakesincompany.at(&who).getamount()!=0)
        return false;
    stakesincompany.erase(&who);
    return true;
}