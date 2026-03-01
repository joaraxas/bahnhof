#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/economy/person.h"
#include "bahnhof/economy/company.h"
#include "bahnhof/ui/panels.h"


bool Portfolio::buy(Portfolio& fromportfolio, Stock& stock, uint16_t amount) {
    if(amount<=0){
        std::cout<<"failed to buy "<<amount<<" shares"<<std::endl;
        return false;
    }
    Stake* const hisstake{stock.getstakeforportfolio(fromportfolio)};
    if(!hisstake){
        std::cout<<"failed to buy shares from non-owner"<<std::endl;
        return false;
    }
    if(!buy(*hisstake, fromportfolio.account, amount))
        return false;
    if(hisstake->getamount() == 0){
        stock.removeemptystake(fromportfolio);
        fromportfolio.stocks.erase(&stock);
    }
    return true;
}

bool Portfolio::buy(Stake& fromstake, Account& payableaccount, uint16_t amount) {
    if(fromstake.getamount()<amount){
        std::cout<<"failed to buy "<<amount<<" shares, owner has only "<<
            fromstake.getamount()<<" shares"<<std::endl;
        return false;
    }
    Stock& stock = fromstake.getstock();
    Money purchaseamount = amount * fromstake.getstock().getshareprice();
    if(!account.canafford(purchaseamount)){
        std::cout<<"failed to buy shares for "<<purchaseamount<<", owner has only "<<
            account.getvalue()<<std::endl;
        return false;
    }
    account.pay(purchaseamount, &payableaccount);
    Stake* mystake = stock.getstakeforportfolio(*this);
    if(!mystake){
        stocks.emplace(&stock);
        mystake = &stock.registernewstake(*this);
    }
    mystake->buyfrom(fromstake, amount);
    return true;
}

bool Stock::issue(Money investment, Portfolio& buyer) {
    uint16_t issuedshares = std::lround(
        std::floor(investment/getshareprice()));
    if(issuedshares<=0){
        std::cout<<"failed to emit "<<issuedshares<<" shares"<<std::endl;
        return false;
    }
    Stake tempstake(*this, issuedshares);
    if(!buyer.buy(tempstake, account, issuedshares))
        return false;
    valuation += getshareprice() * issuedshares;
    shares += issuedshares;
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
            new UI::Economy::InvestorPanel(ui, *this)
        );
    else
        panel.movetofront();
}

void Company::createpanel(InterfaceManager* ui) {
    if(!panel.exists())
        panel.set(
            new UI::Economy::CompanyPanel(ui, stock, getnameforedit(),
                portfolio)
        );
    else
        panel.movetofront();
}