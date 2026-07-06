#include <localization/localization.h>
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/common/math.h"

namespace Economy{

std::string generateslogan() {
    int num = randint(7-1);
    std::string key = "slogans."+std::to_string(num);
    return tr(key);
}

} // namespace Economy