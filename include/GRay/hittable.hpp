#pragma once

#include <GRay/ray.hpp>

namespace GRay::Math
{
    struct hitRecord
    {
        Point3 p;
        Vec3 normal;
        double t;
        bool frontFace;

        inline void setFaceNormal(const Ray& r, const Vec3& outwardNormal)
        {
            frontFace = dot(r.direction(), outwardNormal) < 0;
            normal = frontFace ? outwardNormal : -outwardNormal;
        }
    };

    class Hittable
    {
    public:
        virtual bool hit(const Ray& r, double t_min, double t_max, hitRecord& rec) const = 0;
    };
}