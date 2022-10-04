#pragma once

#include <GRay/rtweekend.hpp>
#include <GRay/texture.hpp>

using namespace GRay;

namespace GRay::Solids
{
    class Background
    {
    public:
        Background(Math::Color c) : mat_ptr{make_shared<Materials::DiffuseLight>(c)} {}
        Background(shared_ptr<Materials::Texture> t) : mat_ptr{make_shared<Materials::DiffuseLight>(t)} {}

        Math::Color getValue(const Math::Ray& ray) const
        {
            double u, v;
            Math::Vec3 unitRay = Math::unitVector(ray.direction());
            getSphereUV(static_cast<Math::Point3>(unitRay), u, v);
            return mat_ptr->emitted(u, v, static_cast<Math::Point3>(unitRay));
        }

    public:
        shared_ptr<GRay::Material> mat_ptr;
    private:
        static void getSphereUV(const Math::Point3& p, double& u, double& v)
        {
            // p: a given point on the sphere of radius one, centered at the origin.
            // u: returned value [0,1] of angle around the Y axis from X=-1.
            // v: returned value [0,1] of angle from Y=-1 to Y=+1.
            //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
            //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
            //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
            auto theta = acos(-p.y());
            auto phi = atan2(-p.z(), p.x()) + Math::pi;
            u = phi / 2 / Math::pi;
            v = theta / Math::pi;
        }
    };
} // namespace GRay::Solids
