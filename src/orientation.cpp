#include "bahnhof/common/orientation.h"

bool Angle::isbetween(const Angle& lower, const Angle& upper)
{
    if(upper._angle>=lower._angle)
        return _angle>=lower._angle && _angle<=upper._angle;
    return _angle>=lower._angle || _angle<=upper._angle;
}

Tangent::Tangent(): Tangent(0)
{}

Tangent::Tangent(float angle): _angle(truncate(angle, pi))
{}

Tangent::Tangent(Angle a): _angle(truncate(a.getradians(), pi))
{}

double Tangent::absanglediff(const Tangent& other) const
{
    // Computes the smallest angle difference between two orientations
    return abs(anglediff(_angle, other._angle, pi));
}

Tangent::operator std::string() const
{
    return std::to_string(_angle);
}

double sin(const Tangent& tangent)
{
    return std::sin(tangent._angle);
}

double cos(const Tangent& tangent)
{
    return std::cos(tangent._angle);
}

double tan(const Tangent& tangent)
{
    return std::tan(tangent._angle);
}