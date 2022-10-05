#pragma once

#include <GRay/rtweekend.hpp>
#include <GRay/aabb.h>

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
        double u;
        double v;
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
        virtual bool boundingBox(double time0, double time1, GRay::Solids::AABB& outputBox) const = 0;
    };

    class Translate : public Hittable
    {
    public:
        Translate(shared_ptr<Hittable> p, Math::Vec3 displacement) : ptr{p}, offset{displacement} {}
        bool hit(const Ray& r, double t_min, double t_max, hitRecord& rec) const override;
        bool boundingBox(double time0, double time1, GRay::Solids::AABB& outputBox) const override;
    public:
        shared_ptr<Hittable> ptr;
        Math::Vec3 offset;
    };

    class RotateY : public Hittable
    {
    public:
        RotateY(shared_ptr<Hittable> p, double angle);
        bool hit(const Ray& r, double t_min, double t_max, hitRecord& rec) const override;
        bool boundingBox(double time0, double time1, GRay::Solids::AABB& outputBox) const override
        {
            outputBox = bbox;
            return hasBox;
        }
    public:
        shared_ptr<Hittable> ptr;
        double sinTheta;
        double cosTheta;
        bool hasBox;
        Solids::AABB bbox;
    };

    bool Translate::hit(const Ray& r, double t_min, double t_max, hitRecord& rec) const 
    {
        Math::Ray movedRay(r.origin() - offset, r.direction(), r.time());
        if (!ptr->hit(movedRay, t_min, t_max, rec))
            return false;
        rec.p += offset;
        rec.setFaceNormal(movedRay, rec.normal);
        return true;
    }

    bool Translate::boundingBox(double time0, double time1, GRay::Solids::AABB& outputBox) const
    {
        if (!ptr->boundingBox(time0, time1, outputBox))
            return false;
        outputBox = Solids::AABB(outputBox.min() + offset, outputBox.max() + offset);
        return true;
    }

    RotateY::RotateY(shared_ptr<Hittable> p, double angle) : ptr(p) 
    {
        auto radians = degreesToRadians(angle);
        sinTheta = sin(radians);
        cosTheta = cos(radians);
        hasBox = ptr->boundingBox(0, 1, bbox);

        Math::Point3 min(infinity, infinity, infinity);
        Math::Point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j)
                for (int k = 0; k < 2; ++k)
                {
                    auto x = i * bbox.max().x() + (1-i) * bbox.min().x();
                    auto y = j * bbox.max().y() + (1-j) * bbox.min().y();
                    auto z = k * bbox.max().z() + (1-k) * bbox.min().z();
                    auto newx = cosTheta*x + sinTheta*z;
                    auto newz = -sinTheta*x + cosTheta*z;

                    Math::Vec3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++)
                    {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
        bbox = Solids::AABB(min, max);
    }

    bool RotateY::hit(const Ray& r, double t_min, double t_max, hitRecord& rec) const 
    {
        auto origin = r.origin();
        auto direction = r.direction();
        
        origin[0] = cosTheta * r.origin()[0] - sinTheta * r.origin()[2];
        origin[2] = sinTheta * r.origin()[0] + cosTheta * r.origin()[2];
        direction[0] = cosTheta * r.direction()[0] - sinTheta * r.direction()[2];
        direction[2] = sinTheta * r.direction()[0] + cosTheta * r.direction()[2];
        
        Math::Ray rotatedRay(origin, direction, r.time());
        if (!ptr->hit(rotatedRay, t_min, t_max, rec))
            return false;
        
        auto p = rec.p;
        auto normal = rec.normal;
        
        p[0] = cosTheta * rec.p[0] - sinTheta * rec.p[2];
        p[2] = sinTheta * rec.p[0] + cosTheta * rec.p[2];
        normal[0] = cosTheta * rec.normal[0] - sinTheta * rec.normal[2];
        normal[2] = sinTheta * rec.normal[0] + cosTheta * rec.normal[2];

        rec.p = p;
        rec.setFaceNormal(rotatedRay, normal);
        return true;
    }
}