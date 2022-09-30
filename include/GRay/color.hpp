#pragma once
#include <GRay/vec3.hpp>

namespace GRay::Colors
{
    void writeColor(std::ostream& out, GRay::Math::Color pixelColor, int samplesPerPixel)
    {
        double r = pixelColor.x();
        double g = pixelColor.y();
        double b = pixelColor.z();
        double scale = 1.0 / samplesPerPixel;
        r = sqrt(r * scale);
        g = sqrt(g * scale);
        b = sqrt(b * scale);
        out << static_cast<int>(256 * GRay::Utils::clamp(r, 0.0, 0.999)) << ' '
            << static_cast<int>(256 * GRay::Utils::clamp(g, 0.0, 0.999)) << ' '
            << static_cast<int>(256 * GRay::Utils::clamp(b, 0.0, 0.999)) << '\n';
    }
}