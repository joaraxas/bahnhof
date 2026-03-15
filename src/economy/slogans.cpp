#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/economy/company.h"
#include "bahnhof/common/math.h"

constexpr auto slogans = std::to_array<const char*> ({
    "Where the wheels keep on turning",
    "Your satisfaction is our obligation",
    "Excellence delivered",
    "Speed and comfort",
    "Delivering value - every day",
    "Full steam ahead!"
});

namespace Economy{

std::string generateslogan() {
    int num = randint(slogans.size()-1);
    return slogans[num];
}

} // namespace Economy