#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/economy/person.h"
#include "bahnhof/economy/thepublic.h"
#include "bahnhof/economy/company.h"
#include "bahnhof/economy/stockmarket.h"
#include "bahnhof/ui/panels.h"

bool Stake::takefrom(Stake& from, int howmany) {
    if(from.stock!=stock) return false;
    if(from.amount>=howmany){
        from.amount -= howmany;
        amount += howmany;
        stock->updateregistry();
        return true;
    }
    return false;
}

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
        std::cout<<"failed to buy "<<amount<<" shares, buyer has only "<<
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
    mystake->takefrom(fromstake, amount);
    return true;
}

Stock::Stock(Entity& e, Account& a, Stockmarket& sm) : 
        entity{e}, account{a}, market{sm}
{
    market.liststock(*this);
}

Stock::~Stock()
{
    market.deliststock(*this);
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
    auto [p, dummy] = stakes.emplace(&who, Stake(*this));
    Stake& newstake = p->second;
    sortedowners.emplace_back(&newstake, &who.getentity());
    return newstake;
}

bool Stock::removeemptystake(Portfolio& who) {
    if(!stakes.contains(&who))
        return true;
    Stake& stake = stakes.at(&who);
    if(stake.getamount()!=0)
        return false;
    for(auto p = sortedowners.rbegin(); p!=sortedowners.rend(); p++) {
        if(p->first == &stake) {
            sortedowners.erase(std::next(p).base(), sortedowners.end());
            break;
        }
    }
    stakes.erase(&who);
    return true;
}

bool compareowners(const std::pair<Stake*, Entity*>& a, 
                     const std::pair<Stake*, Entity*>& b)
{
    if(*a.first > *b.first)
        return true;
    if(*a.first < *b.first)
        return false;
    return a.second->getname() < b.second->getname();
}

void Stock::updateregistry() {
    std::sort(sortedowners.begin(), sortedowners.end(), compareowners);
}

void Person::createpanel(InterfaceManager* ui) {
    if(!panel.exists())
        panel.set(
            new UI::Economy::InvestorPanel(ui, name, portfolio, account)
        );
    else
        panel.movetofront();
}

void ThePublic::createpanel(InterfaceManager* ui) {
    if(!panel.exists())
        panel.set(
            new UI::Economy::ThePublicPanel(ui, name, portfolio)
        );
    else
        panel.movetofront();
}

void Company::createpanel(InterfaceManager* ui) {
    if(!panel.exists())
        panel.set(
            new UI::Economy::CompanyPanel(ui, stock, getnameforedit(),
                portfolio, account)
        );
    else
        panel.movetofront();
}

void Stockmarket::createpanel(InterfaceManager* ui) {
    if(!panel.exists())
        panel.set(
            new UI::Economy::StockmarketPanel(ui, stocks)
        );
    else
        panel.movetofront();
}