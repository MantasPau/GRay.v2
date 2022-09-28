#pragma once
#include <GRay.v2/vec3.hpp>

namespace GRay::Color
{
    void writeColor(std::ostream& out, GRay::Math::Color pixelColor)
    {
        out << static_cast<int>(255.999 * pixelColor.x()) << ' '
            << static_cast<int>(255.999 * pixelColor.y()) << ' '
            << static_cast<int>(255.999 * pixelColor.z()) << '\n';
    }
}