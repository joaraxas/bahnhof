#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "entity.h"
#include "account.h"
#include "stake.h"
#include "owner.h"

class Game;
class Rendering;
class InterfaceManager;

class Company
{
public:
    Company(Entity& e, Account& a) : entity{e}, account{a} {}
    bool emission(Money investment, Owner& buyer);
    Money getvalue() const {return valuation;}
    Money getshareprice() const {
        if(shares == 0) return 5;
        return valuation/shares;
    }
    uint16_t getnumshares() const {return shares;}
    Stake* const getstakeforowner(Owner& who);
    Stake& registernewstake(Owner& who);
    bool removeemptystake(Owner& who);
    const std::map<Owner*,Stake>& getowners() {return stakesincompany;}
    Entity& getentity() {return entity;}
private:
    Entity& entity;
    Account& account;
    uint16_t shares{0};
    std::map<Owner*,Stake> stakesincompany;
    Money valuation{0};
};

class NewCompany : Entity
{
public:
    NewCompany(std::string n) : NewCompany(n, 0) {}
    NewCompany(std::string n, Money startamount) : 
        name{n}, account{startamount}, owner{*this, account}, shares{*this, account} {}
    ~NewCompany() {}
    // bool emission(Money investment, NewCompany& buyer);
    const std::string& getname() const override {return name;}
    Owner& getcompanysinvestments() {return owner;} // might remove
    Company& getcompanysshares() {return shares;} // might remove
    Account& getcompanysaccount() {return account;} // might remove
    void createpanel(InterfaceManager* ui);
private:
    std::string& getnameforedit() {return name;}
    std::string name;
    Account account;
    Owner owner;
    Company shares;
    UI::Ownership panel;
};

// class Company
// {
// public:
//     Company(std::string name) : 
//         owner(name) {};
//     const std::string& getname() const {return owner.getname();}
//     Account& getaccount() {return owner.getaccount();}
//     Money getvalue() const {return valuation;}
//     Money getshareprice() const {
//         if(shares == 0) return 5;
//         return valuation/shares;
//     }
//     uint16_t getnumshares() const {return shares;}
//     void createmainpanel(InterfaceManager* ui);
//     bool emission(Money investment, Owner& buyer);
//     Stake* const getstakeforowner(Owner& who);
//     Stake& registernewstake(Owner& who);
//     bool removeemptystake(Owner& who);
//     Owner& getcompanysinvestments() {return owner;} // might remove
//     const std::map<Owner*,Stake>& getowners() {return stakesincompany;}
// private:
//     std::string& getnameforedit() {return owner.getnameforedit();}
//     Owner owner;
//     uint16_t shares{0};
//     std::map<Owner*,Stake> stakesincompany;
//     Money valuation{0};
//     UI::Ownership mainpanel;
// };