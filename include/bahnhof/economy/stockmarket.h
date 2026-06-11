#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"

class InterfaceManager;

namespace Economy {

class Stock;

class Stockmarket
{
public:
    void update(int ms);
    void liststock(Stock& stock) {stocks.push_back(&stock);}
    bool deliststock(Stock& stock) {return std::erase(stocks, &stock);}
    void createpanel(InterfaceManager* ui);
private:
    std::vector<Stock*> stocks;
    UI::Ownership panel;
    int timesincelastupdate_ms;
};


} // namespace Economy