#pragma once

#include <GRay/rtweekend.hpp>

namespace GRay::Materials
{
    class Texture
    {
    public:
        virtual GRay::Math::Color value(double u, double v, const GRay::Math::Point3& p) const = 0;
    };

    class SolidColor : public Texture
    {
    public:
        SolidColor() {}
        SolidColor(GRay::Math::Color c) : colorValue{c} {}
        SolidColor(double red, double green, double blue) : SolidColor(GRay::Math::Color(red, green, blue)) {}
        GRay::Math::Color value(double u, double v, const GRay::Math::Point3& p) const override {return colorValue;}
    public:
        GRay::Math::Color colorValue;
    };

    class CheckerTexture : public Texture
    {
    public:
        CheckerTexture() {}
        CheckerTexture(shared_ptr<Texture> _even, shared_ptr<Texture> _odd) : odd{_odd}, even{_even} {}
        CheckerTexture(GRay::Math::Color c1, GRay::Math::Color c2) : even{make_shared<SolidColor>(c1)}, odd{make_shared<SolidColor>(c2)} {}
        GRay::Math::Color value(double u, double v, const GRay::Math::Point3& p) const override
        {
            auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
            if (sines < 0)
                return odd->value(u, v, p);
            else
                return even->value(u, v, p);
        }

    public:
        shared_ptr<Texture> odd;
        shared_ptr<Texture> even;        
    };
}