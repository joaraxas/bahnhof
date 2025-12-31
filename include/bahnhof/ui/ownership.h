#pragma once
#include "bahnhof/common/forwardincludes.h"

namespace UI{

class Host;

class Ownership
{
public:
    ~Ownership();
    void set(Host* newhost);
    bool exists();
    void deletereference();
    void resetreference();
private:
    Host* host = nullptr;
};

} // namespace UI