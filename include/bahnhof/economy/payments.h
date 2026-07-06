#pragma once
#include <localization/localization.h>
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
        return tr("payments.stocks");
    case PaymentType::rollingstock:
        return tr("payments.rollingstock");
    case PaymentType::buildings:
        return tr("payments.buildings");
    case PaymentType::ticketfare:
        return tr("payments.tickets");
    default:
        return tr("payments.unknown");
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

using PaymentList = std::map<Economy::PaymentType, Economy::Money>;

} // namespace Economy