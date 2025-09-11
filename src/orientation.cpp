#include "bahnhof/common/orientation.h"

Tangent::Tangent(): _angle(0)
{}

Tangent::Tangent(float angle): _angle(truncate(angle))
{}

float Tangent::operator -(const Tangent& other)
{
    // Computes the smallest angle difference between two orientations
    return anglediff(_angle, other._angle, pi);
}