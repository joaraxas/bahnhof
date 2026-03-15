#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "money.h"


namespace Economy {

enum class PaymentType {
    stocks,
    rollingstock,
    buildings,
    ticketfare
};

constexpr std::string getbudgetpostname(PaymentType type)
{
    switch (type)
    {
    case PaymentType::stocks:
        return "Stock investments";
    case PaymentType::rollingstock:
        return "Rolling stock";
    case PaymentType::buildings:
        return "Buildings";
    case PaymentType::ticketfare:
        return "Ticket sales";
    default:
        return "Unknown";
    }
}

constexpr bool isrecurring(PaymentType type)
{
    switch (type)
    {
    case PaymentType::ticketfare:
        return true;
    default:
        return false;
    }
}

using Typelist = std::map<Economy::PaymentType, Economy::Money>;

} // namespace Economy