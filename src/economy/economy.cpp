#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/economy/company.h"
#include "bahnhof/economy/stake.h"
#include "bahnhof/economy/owner.h"
#include "bahnhof/economy/money.h"
#include "bahnhof/ui/panels.h"


bool Portfolio::buy(Portfolio& fromportfolio, Stock& stock, uint16_t amount) {
    if(amount<=0)
        return false;
    Stake* const hisstake{stock.getstakeforportfolio(fromportfolio)};
    if(!hisstake)
        return false;
    if(!buy(*hisstake, fromportfolio.account, amount))
        return false;
    if(hisstake->getamount() == 0){
        stock.removeemptystake(fromportfolio);
        fromportfolio.stocks.erase(&stock);
    }
    return true;
}

bool Portfolio::buy(Stake& fromstake, Account& payableaccount, uint16_t amount) {
    if(fromstake.getamount()<amount)
        return false;
    Stock& company = fromstake.getstock();
    Money purchaseamount = amount * fromstake.getstock().getshareprice();
    if(!account.canafford(purchaseamount))
        return false;
    account.pay(purchaseamount, &payableaccount);
    Stake* mystake = company.getstakeforportfolio(*this);
    if(!mystake){
        stocks.emplace(&company);
        mystake = &company.registernewstake(*this);
    }
    mystake->buyfrom(fromstake, amount);
    return true;
}

bool Stock::emission(Money investment, Portfolio& buyer) {
    uint16_t emittedshares = std::lround(
        std::floor(investment/getshareprice()));
    if(emittedshares==0)
        return false;
    Stake tempstake(*this, emittedshares);
    if(!buyer.buy(tempstake, account, emittedshares))
        return false;
    valuation += getshareprice() * emittedshares;
    shares += emittedshares;
    return true;
}

Stake* const Stock::getstakeforportfolio(Portfolio& who) {
    if(!stakes.contains(&who))
        return nullptr;
    return &stakes.at(&who);
}

Stake& Stock::registernewstake(Portfolio& who) {
    auto result = stakes.emplace(&who, Stake(*this));
    return result.first->second;
}

bool Stock::removeemptystake(Portfolio& who) {
    if(!stakes.contains(&who))
        return true;
    if(stakes.at(&who).getamount()!=0)
        return false;
    stakes.erase(&who);
    return true;
}

void Person::createpanel(InterfaceManager* ui) {
    if(!panel.exists())
        panel.set(
            new UI::InvestorPanel(ui, portfolio, name)
        );
    else
        panel.movetofront();
}


void NewCompany::createpanel(InterfaceManager* ui) {
    if(!panel.exists())
        panel.set(
            new UI::CompanyPanel(ui, shares, getnameforedit(),
                owner)
        );
    else
        panel.movetofront();
}