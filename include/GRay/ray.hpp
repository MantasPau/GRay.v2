#pragma once

#include <GRay/vec3.hpp>

namespace GRay::Math
{
    class Ray
    {
    public:
        Ray() {}
        Ray(const Point3& origin, const Vec3& direction, double time = 0.0) : orig{origin}, dir{direction}, tm{time} {}
        const Point3& origin() const {return orig;}
        const Vec3& direction() const {return dir;}
        double time() const {return tm;}

        Point3 at(double t) const
        {
            return orig + t*dir;
        }

    public:
        Point3 orig;
        Point3 dir;
        double tm;
    };
}