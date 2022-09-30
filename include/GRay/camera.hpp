#pragma once

#include <GRay/rtweekend.hpp>

namespace GRay
{
    using namespace GRay::Math;

    class Camera
    {
    public:
        Camera(Point3 lookFrom, Point3 lookAt, Vec3 vUp, double vfov, double aspectRatio, double apperture, double focusDist)
        {
            double theta = degreesToRadians(vfov);
            double h = tan(theta / 2);
            double viewportHeight = 2.0 * h;
            double viewportWidth = aspectRatio * viewportHeight;
            
            w = unitVector(lookFrom - lookAt);
            u = unitVector(cross(vUp, w));
            v = cross(w, u);

            origin = lookFrom;
            horizontal = focusDist * viewportWidth * u;
            vertical = focusDist * viewportHeight * v;
            lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - focusDist * w;

            lensRadius = apperture / 2;
        }

        Ray getRay(double s, double t) const
        {
            Vec3 rd = lensRadius * randomInUnitDisc();
            Vec3 offset = u * rd.x() + v * rd.y();
            return Ray(origin + offset, lowerLeftCorner + s*horizontal + t*vertical - origin - offset);
        }
    private:
        Point3 origin;
        Point3 lowerLeftCorner;
        Vec3 horizontal;
        Vec3 vertical;
        Vec3 u, v, w;
        double lensRadius;
    };
}