#pragma once

#include <GRay/rtweekend.hpp>
#include <GRay/hittable.hpp>
#include <GRay/hittableList.hpp>
#include <algorithm>

namespace GRay::Solids
{
    class BvhNode : public GRay::Math::Hittable
    {
    public:
        BvhNode();
        BvhNode(const GRay::Math::HittableList& list, double time0, double time1) :
                BvhNode(list.objects, 0, list.objects.size(), time0, time1) {}
        BvhNode(const std::vector<shared_ptr<GRay::Math::Hittable> >& srcObjects, 
                size_t start, size_t end, double time0, double time1);
        bool hit(const GRay::Math::Ray& r, double t_min, double t_max, GRay::Math::hitRecord& rec) const override;
        bool boundingBox(double time0, double time1, GRay::Solids::AABB& outputBox) const override;
    public:
        shared_ptr<GRay::Math::Hittable> left;
        shared_ptr<GRay::Math::Hittable> right;
        AABB box;
    };

    bool BvhNode::boundingBox(double time0, double time1, GRay::Solids::AABB& outputBox) const
    {
        outputBox = box;
        return true;
    }

    bool BvhNode::hit(const GRay::Math::Ray& r, double t_min, double t_max, GRay::Math::hitRecord& rec) const
    {
        if (!box.hit(r, t_min, t_max))
            return false;

        bool hitLeft = left->hit(r, t_min, t_max, rec);
        bool hitRight = right->hit(r, t_min, hitLeft ? rec.t : t_max, rec);

        return hitLeft || hitRight;
    }

    BvhNode::BvhNode(const std::vector<shared_ptr<GRay::Math::Hittable> >& srcObjects, size_t start, size_t end, double time0, double time1)
    {
        auto objects = srcObjects;
        int axis = Utils::randomInt(0, 2);
        auto comparator = (axis == 0) ? boxXCompare : (axis == 1) ? boxYCompare : boxZCompare;

        size_t objectSpan = end - start;

        if (objectSpan == 1)
        {
            left = right = objects[start];
        }
        else if (objectSpan == 2)
        {
            if (comparator(objects[start], objects[start+1]))
            {
                left = objects[start];
                right = objects[start+1];
            }
            else
            {
                left = objects[start+1];
                right = objects[start];
            }
        }
        else
        {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);
            auto mid = start + objectSpan/2;
            left = make_shared<BvhNode>(objects, start, mid, time0, time1);
            right = make_shared<BvhNode>(objects, mid, end, time0, time1);
        }

        AABB boxLeft, boxRight;
        if (!left->boundingBox(time0, time1, boxLeft) || !right->boundingBox(time0, time1, boxRight))
            std::cerr << "No bounding box in BvhNode constructor.\n";
        box = surroundingBox(boxLeft, boxRight);
    }

    inline bool boxComapre(const shared_ptr<GRay::Math::Hittable> a, const shared_ptr<GRay::Math::Hittable> b, int axis)
    {
        AABB boxA;
        AABB boxB;

        if (!a->boundingBox(0, 0, boxA) || !b->boundingBox(0, 0, boxB))
            std::cerr << "No bounding box in  bvhNode constructor";
        return boxA.min().e[axis] < boxB.min().e[axis];
    }

    bool boxXCompare(const shared_ptr<GRay::Math::Hittable> a, const shared_ptr<GRay::Math::Hittable> b)
    {
        return boxComapre(a, b, 0);
    }

    bool boxYCompare(const shared_ptr<GRay::Math::Hittable> a, const shared_ptr<GRay::Math::Hittable> b)
    {
        return boxComapre(a, b, 1);
    }

    bool boxZCompare(const shared_ptr<GRay::Math::Hittable> a, const shared_ptr<GRay::Math::Hittable> b)
    {
        return boxComapre(a, b, 2);
    }
}