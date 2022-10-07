#pragma once

#include <GRay/rtweekend.hpp>
#include <GRay/aarect.hpp>
#include <GRay/hittableList.hpp>

using namespace GRay;

namespace GRay
{
    namespace Solids
    {
        class Box : public Math::Hittable
        {
        public:
            Box() {}
            Box(const Math::Point3& p0, const Math::Point3& p1, shared_ptr<Material> mat_ptr);

            bool hit(const Ray& r, double t_min, double t_max, hitRecord& rec) const override;
            bool boundingBox(double time0, double time1, GRay::Solids::AABB& outputBox) const override
            {
                outputBox = AABB(boxMin, boxMax);
                return true;
            }
        public:
            Math::Point3 boxMin;
            Math::Point3 boxMax;
            Math::HittableList sides;
        };

        Box::Box(const Math::Point3& p0, const Math::Point3& p1, shared_ptr<Material> mat_ptr)
        {
            boxMin = p0;
            boxMax = p1;

            sides.add(make_shared<XYRect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), mat_ptr));
            sides.add(make_shared<XYRect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), mat_ptr));

            sides.add(make_shared<XZRect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), mat_ptr));
            sides.add(make_shared<XZRect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), mat_ptr));

            sides.add(make_shared<YZRect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), mat_ptr));
            sides.add(make_shared<YZRect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), mat_ptr));
        }

        bool Box::hit(const Ray& r, double t_min, double t_max, hitRecord& rec) const
        {
            return sides.hit(r, t_min, t_max, rec);
        }
    }
}