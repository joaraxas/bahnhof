#pragma once
#include "bahnhof/economy/control.h"

class Building;
class Storage;

namespace Economy{

class Account;
class Portfolio;

struct ControlMode
{
    ControlMode() = default;
    ControlMode(Entity* e, Account* a, Portfolio* p, Control<Building>* b, Control<Storage>* s) :
        entity{e},
        account{a},
        portfolio{p},
        buildings{b},
        storages{s}
        {}
    Entity* entity = nullptr;
    Account* account = nullptr;
    Portfolio* portfolio = nullptr;
    Control<Building>* buildings = nullptr;
    Control<Storage>* storages = nullptr;
};

} // namespace Economy