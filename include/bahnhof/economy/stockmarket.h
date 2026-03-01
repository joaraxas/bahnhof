#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "stock.h"

class InterfaceManager;

class Stockmarket
{
public:
    void liststock(Stock& stock) {stocks.push_back(&stock);}
    bool deliststock(Stock& stock) {return std::erase(stocks, &stock);}
    void createpanel(InterfaceManager* ui);
private:
    std::vector<Stock*> stocks;
    UI::Ownership panel;
};
