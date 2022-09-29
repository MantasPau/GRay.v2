#pragma once

#include <GRay/rtweekend.hpp>

namespace GRay
{
    using namespace GRay::Math;

    class Camera
    {
    public:
        Camera()
        {
            const double aspectRatio = 16.0 / 9.0;
            double viewportHeight = 2.0;
            double viewportWidth = aspectRatio * viewportHeight;
            double focalLength = 1.0;

            origin = Point3(0, 0, 0);
            horizontal = Vec3(viewportWidth, 0, 0);
            vertical = Vec3(0, viewportHeight, 0);
            lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - Vec3(0, 0, focalLength);
        }

        Ray getRay(double u, double v) const
        {
            return Ray(origin, lowerLeftCorner + u*horizontal + v*vertical - origin);
        }
    private:
        Point3 origin;
        Point3 lowerLeftCorner;
        Vec3 horizontal;
        Vec3 vertical;
    };
}