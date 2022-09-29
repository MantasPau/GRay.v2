#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

//Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

namespace GRay::Utils
{
    //Constants
    const double infinity = std::numeric_limits<double>::infinity();
    const double pi = M_PI;

    //Utility functions

    inline double degreesToRadians(double degrees)
    {
        return degrees * pi / 180.0;
    }

    inline double randomDouble()
    {
        return rand() / (RAND_MAX + 1.0);
    }

    inline double randomDouble(double min, double max)
    {
        return min + (max - min) * randomDouble();
    }

    inline double clamp(double x, double min, double max)
    {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }
}