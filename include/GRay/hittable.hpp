#pragma once

#include <GRay/rtweekend.hpp>

namespace GRay
{
    class Material;
}

namespace GRay::Math
{
    struct hitRecord
    {
        Point3 p;
        Vec3 normal;
        shared_ptr<GRay::Material> mat_ptr;
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