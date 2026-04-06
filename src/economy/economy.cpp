#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/economy/person.h"
#include "bahnhof/economy/thepublic.h"
#include "bahnhof/economy/company.h"
#include "bahnhof/economy/stockmarket.h"
#include "bahnhof/economy/control.h"
#include "bahnhof/ui/panels.h"
#include "bahnhof/buildings/buildings.h"


namespace Economy{
    
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
    amount = std::min(amount, fromstake.getamount());
    if(amount<=0) {
        std::cout<<"failed to buy "<<amount<<" shares"<<std::endl;
        return false;
    }
    Stock& stock = fromstake.getstock();
    Money purchaseamount = amount * fromstake.getstock().getshareprice();
    if(!account.canafford(purchaseamount)){
        std::cout<<"failed to buy shares for "<<purchaseamount<<", owner has only "<<
            account.getvalue()<<std::endl;
        return false;
    }
    account.pay(purchaseamount, PaymentType::stocks, &payableaccount);
    Stake* mystake = stock.getstakeforportfolio(*this);
    if(!mystake){
        stocks.emplace(&stock);
        mystake = &stock.registernewstake(*this);
    }
    mystake->takefrom(fromstake, amount);
    return true;
}

bool Portfolio::isplayercontrolled()
{
    return playercontrol.is;
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

void Stock::update(int ms)
{
    constexpr double stdpersec = 0.05/60.0;
    valuation *= randnorm(stdpersec*0.001*ms, 1.0);
    Money currentprofit = account.getprofit();
    Money newprofit = currentprofit - lastprofit;
    valuation += newprofit * 5;
    lastprofit = currentprofit;
}

bool Stock::issue(Money investment, Portfolio& buyer) {
    constexpr double devaluation = 0.95;
    Money oldvaluation = valuation;
    valuation *= devaluation;
    uint16_t issuedshares = std::lround(
        std::floor(investment/getshareprice()));
    if(issuedshares<=0){
        std::cout<<"failed to emit "<<issuedshares<<" shares"<<std::endl;
        valuation = oldvaluation;
        return false;
    }
    Stake tempstake(*this, issuedshares);
    if(!buyer.buy(tempstake, account, issuedshares)){
        valuation = oldvaluation;
        return false;
    }
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
    // no need to sort because the stake is empty.
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

bool Stock::attempttakeover() {
    uint16_t votesfor{0};
    for(auto& [portfolio, stake] : stakes){
        if(portfolio->isplayercontrolled()){
            std::cout<<stake.getamount()<<" votes for"<<std::endl;
            votesfor+=stake.getamount();
        }
    }
    if(votesfor>shares*0.5){
        std::cout<<"success!"<<std::endl;
        return true;
    }
    std::cout<<"failure"<<std::endl;
    return false;
}

void Person::createpanel(InterfaceManager* ui) {
    if(!panel.exists())
        panel.set(
            new UI::EconomyPanels::InvestorPanel(ui, name, portfolio, account)
        );
    else
        panel.movetofront();
}

void ThePublic::createpanel(InterfaceManager* ui) {
    if(!panel.exists())
        panel.set(
            new UI::EconomyPanels::ThePublicPanel(ui, getname(), portfolio)
        );
    else
        panel.movetofront();
}

void Company::createpanel(InterfaceManager* ui) {
    if(!panel.exists())
        panel.set(
            new UI::EconomyPanels::CompanyPanel(ui, stock, name,
                slogan, portfolio, account, buildings, 
                ControllerPointerDirect(playercontrol))
        );
    else
        panel.movetofront();
}


void Stockmarket::update(int ms) {
    timesincelastupdate_ms += ms;
    if(timesincelastupdate_ms > 1000){
        timesincelastupdate_ms -= 1000;
        for(Stock* stock: stocks) 
            stock->update(1000);
    }
}

void Stockmarket::createpanel(InterfaceManager* ui) {
    if(!panel.exists())
        panel.set(
            new UI::EconomyPanels::StockmarketPanel(ui, stocks)
        );
    else
        panel.movetofront();
}

template<>
void Control<Building>::createpanel(InterfaceManager* ui) {
    if(!panel.exists())
        panel.set(
            new UI::EconomyPanels::PossessionsPanel<Building>(ui, possessions)
        );
    else
        panel.movetofront();
}

template<>
bool buy(Building& building, Control<Building>& buyer, Money price) {
    auto& owner = building.getowner();
    if(&owner == &buyer) {
        std::cout<<owner.getentity().getname()<<
            " tried to buy "<<building.getname()<<" from himself"<<std::endl;
        return false;
    }
    if(!buyer.getaccount().pay(price, PaymentType::buildings, &owner.getaccount()))
        return false;
    owner.delistpossession(building); 
    buyer.listpossession(building); 
    building.setowner(buyer);
    return true;
};

void Account::createpanel(InterfaceManager* ui) {
    if(!panel.exists())
        panel.set(
            new UI::EconomyPanels::AccountPanel(ui, income, expenses)
        );
    else
        panel.movetofront();
}

} // namespace Economy