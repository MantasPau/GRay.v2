#pragma once

#include <GRay/hittable.hpp>
#include <GRay/aabb.h>
#include <memory>
#include <vector>

namespace GRay::Math
{
    class HittableList : public Hittable
    {
    public:
        HittableList() {}
        HittableList(std::shared_ptr<Hittable> object) {add(object);}

        void clear() {objects.clear();}
        void add(std::shared_ptr<Hittable> object) {objects.push_back(object);}
        bool hit(const Ray& r, double t_min, double t_max, hitRecord& rec) const override;
        bool boundingBox(double time0, double time1, GRay::Solids::AABB& outputBox) const override;
    public:
        std::vector<std::shared_ptr<Hittable> > objects;
    };

    bool HittableList::hit(const Ray& r, double t_min, double t_max, hitRecord& rec) const
    {
        hitRecord tmpRec;
        bool hitAnything = false;
        double closestHitSoFar = t_max;

        for (const std::shared_ptr<Hittable>& object : objects)
        {
            if (object->hit(r, t_min, closestHitSoFar, tmpRec))
            {
                hitAnything = true;
                closestHitSoFar = tmpRec.t;
                rec = tmpRec;
            }
        }
        return hitAnything;
    }

    bool HittableList::boundingBox(double time0, double time1, GRay::Solids::AABB& outputBox) const
    {
        if (objects.empty()) return false;

        GRay::Solids::AABB tempBox;
        bool firstBox = true;

        for (const auto& object : objects)
        {
            if (!object->boundingBox(time0, time1, tempBox)) return false;
            outputBox = firstBox ? tempBox : GRay::Solids::suroundingBox(outputBox, tempBox);
            firstBox = false;
        }

        return true;
    }
}