#pragma once

#include <GRay/rtweekend.hpp>
#include <GRay/hittable.hpp>
#include <GRay/material.hpp>
#include <GRay/texture.hpp>

using namespace GRay;

namespace GRay
{
    namespace Solids
    {
        class ConstantMedium : public Math::Hittable
        {
        public:
            ConstantMedium(shared_ptr<Math::Hittable> b, double d, shared_ptr<Materials::Texture> a) :
                boundary{ b }, negInvDensity{ -1 / d }, phaseFunction(make_shared<Materials::Isotropic>(a)) {}
            ConstantMedium(shared_ptr<Math::Hittable> b, double d, Math::Color c) :
                boundary{ b }, negInvDensity{ -1 / d }, phaseFunction(make_shared<Materials::Isotropic>(c)) {}
            bool hit(const Math::Ray& r, double t_min, double t_max, Math::hitRecord& rec) const override;
            bool boundingBox(double time0, double time1, GRay::Solids::AABB& outputBox) const override
            {
                return boundary->boundingBox(time0, time1, outputBox);
            }
        public:
            shared_ptr<Math::Hittable> boundary;
            shared_ptr<Material> phaseFunction;
            double negInvDensity;
        };

        bool ConstantMedium::hit(const Math::Ray& r, double t_min, double t_max, Math::hitRecord& rec) const
        {
            const bool enableDebug = false;
            const bool debugging = enableDebug && Utils::randomDouble() < 0.00001;

            Math::hitRecord rec1, rec2;
            if (!boundary->hit(r, -Math::infinity, Math::infinity, rec1))
                return false;
            if (!boundary->hit(r, rec1.t + 0.0001, Math::infinity, rec2))
                return false;

            if (debugging) std::cerr << "\nt_min=" << rec1.t << ", t_max=" << rec2.t << '\n';

            if (rec1.t < t_min) rec1.t = t_min;
            if (rec2.t > t_max) rec2.t = t_max;

            if (rec1.t >= rec2.t)
                return false;

            if (rec1.t < 0)
                rec1.t = 0;

            const auto rayLength = r.direction().length();
            const auto distanceInsideBoundary = (rec2.t - rec1.t) * rayLength;
            const auto hitDistance = negInvDensity * log(Math::randomDouble());

            if (hitDistance > distanceInsideBoundary)
                return false;

            rec.t = rec1.t + hitDistance / rayLength;
            rec.p = r.at(rec.t);

            if (debugging)
            {
                std::cerr << "histDistance = " << hitDistance << '\n'
                    << "rec.t = " << rec.t << '\n'
                    << "rec.p" << rec.p << '\n';
            }

            rec.normal = Math::Vec3(1, 0, 0);
            rec.frontFace = true;
            rec.mat_ptr = phaseFunction;

            return true;
        }
    }
}