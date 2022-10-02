#pragma once

#include <GRay/rtweekend.hpp>
#include <GRay/hittable.hpp>

namespace GRay::Solids
{
    class MovingSphere : public GRay::Math::Hittable
    {
    public:
        MovingSphere() {}
        MovingSphere(GRay::Math::Point3 cen0, GRay::Math::Point3 cen1, double _time0, double _time1, double r, shared_ptr<GRay::Material> m) :
                    center0{cen0}, center1{cen1}, time0{_time0}, time1{_time1}, radius{r}, matPtr{m} {}

        bool hit(const GRay::Math::Ray& r, double t_min, double t_max, GRay::Math::hitRecord& rec) const override
        {
            Math::Vec3 oc = r.origin() - center(r.time());
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
            Math::Vec3 outwardNormal = (rec.p - center(r.time())) / radius;
            rec.setFaceNormal(r, outwardNormal);
            rec.mat_ptr = matPtr;
            return true;
        }
        bool boundingBox(double _time0, double _time1, GRay::Solids::AABB& outputBox) const override;
        GRay::Math::Point3 center(double time) const;
    public:
        GRay::Math::Point3 center0, center1;
        double time0, time1;
        double radius;
        shared_ptr<GRay::Material> matPtr;
    };

    GRay::Math::Point3 MovingSphere::center(double time) const
    {
        return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
    }

    bool MovingSphere::boundingBox(double _time0, double _time1, GRay::Solids::AABB& outputBox) const
    {
        AABB box0(center(_time0) - GRay::Math::Vec3(radius, radius, radius),
                  center(_time0) + GRay::Math::Vec3(radius, radius, radius));
        AABB box1(center(_time1) - GRay::Math::Vec3(radius, radius, radius),
                  center(_time1) + GRay::Math::Vec3(radius, radius, radius));
        outputBox = surroundingBox(box0, box1);
        return true;
    }
}