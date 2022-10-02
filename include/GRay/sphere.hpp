#pragma once

#include <GRay/hittable.hpp>
#include <GRay/material.hpp>

namespace GRay::Solids
{
    class Sphere : public GRay::Math::Hittable
    {
    public:
        Sphere() {}
        Sphere(Math::Point3 cen, double r, shared_ptr<GRay::Material> m) : center{cen}, radius{r}, mat_ptr{m} {}

        bool hit(const Math::Ray& r, double t_min, double t_max, Math::hitRecord& rec) const override;
        bool boundingBox(double time0, double time1, GRay::Solids::AABB& outputBox) const override;
    public:
        Math::Point3 center;
        double radius;
        shared_ptr<GRay::Material> mat_ptr;
    };

    bool Sphere::hit(const Math::Ray& r, double t_min, double t_max, Math::hitRecord& rec) const
    {
        Math::Vec3 oc = r.origin() - center;
        double a = r.direction().lenghtSquared();
        double half_b = Math::dot(oc, r.direction());
        double c = oc.lenghtSquared() - radius * radius;
        double discriminant = half_b*half_b - a*c;

        if (discriminant < 0)
            return false;
        
        double sqrtd = sqrt(discriminant);
        double root = (-half_b - sqrtd) / a;
        if ((root < t_min) || (t_max < root))
        {
            root = (-half_b + sqrtd) / a;
            if ((root < t_min) || (t_max < root))
                return false;
        }
        rec.t = root;
        rec.p = r.at(rec.t);
        Math::Vec3 outwardNormal = (rec.p - center) / radius;
        rec.setFaceNormal(r, outwardNormal);
        rec.mat_ptr = mat_ptr;
        return true;
    }

    bool Sphere::boundingBox(double time0, double time1, GRay::Solids::AABB& outputBox) const
    {
        outputBox = AABB(center - Math::Vec3(radius, radius, radius), center + Math::Vec3(radius, radius, radius));
        return true;
    }
}