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

inline std::string getbudgetpostname(PaymentType transaction)
{
    switch (transaction)
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

using Typelist = std::map<Economy::PaymentType, Economy::Money>;

} // namespace Economy