#pragma once

#include <GRay/rtweekend.hpp>
#include <GRay/math.hpp>
#include <GRay/ray.hpp>
#include <GRay/hittable.hpp>

namespace GRay
{
    namespace Math
    {
        struct hitRecord;
    }
    class Material
    {
    public:
        virtual bool scatter(const Math::Ray& r_in, const Math::hitRecord& rec, Math::Color& attenuation, Math::Ray& scattered) const = 0;
    };

    namespace Materials
    {
        class Lambertian : public Material
        {
        public:
            Lambertian(const GRay::Math::Color& a) : albedo(a) {}
            virtual bool scatter(const GRay::Math::Ray& r_in, const GRay::Math::hitRecord& rec, GRay::Math::Color& attenuation, GRay::Math::Ray& scattered) const override
            {
                GRay::Math::Vec3 scatterDirection = rec.normal + GRay::Math::randomUnitVector();
                if (scatterDirection.nearZero())
                    scatterDirection = rec.normal;
                scattered = GRay::Math::Ray(rec.p, scatterDirection);                
                attenuation = albedo;
                return true;
            }
        public:
            GRay::Math::Color albedo;
        };

        class Metal : public Material
        {
        public:
            Metal(const GRay::Math::Color& a, double f) : albedo{a}, fuzz{f} {}

            virtual bool scatter(const GRay::Math::Ray& r_in, const GRay::Math::hitRecord& rec, GRay::Math::Color& attenuation, GRay::Math::Ray& scattered) const override
            {
                GRay::Math::Vec3 reflected = GRay::Math::reflect(GRay::Math::unitVector(r_in.direction()), rec.normal);
                scattered = GRay::Math::Ray(rec.p, reflected + fuzz * GRay::Math::randomInUnitSphere());
                attenuation = albedo;
                return (GRay::Math::dot(scattered.direction(), rec.normal) > 0);;
            }
        public:
            GRay::Math::Color albedo;
            double fuzz;
        };

        class Dialectric : public Material
        {
        public:
            Dialectric(double indexOfRefraction) : ir{indexOfRefraction} {}
            virtual bool scatter(const GRay::Math::Ray& r_in, const GRay::Math::hitRecord& rec, GRay::Math::Color& attenuation, GRay::Math::Ray& scattered) const override
            {
                attenuation = GRay::Math::Color(1.0, 1.0, 1.0);
                double refractionRatio = rec.frontFace ? (1.0 / ir) : ir;
                GRay::Math::Vec3 unitDirection = GRay::Math::unitVector(r_in.direction());
                double cosTheta = fmin(GRay::Math::dot(-unitDirection, rec.normal), 1.0);
                double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

                bool cannotRefract = refractionRatio * sinTheta > 1;
                GRay::Math::Vec3 direction;
                if(cannotRefract)
                    direction = GRay::Math::reflect(unitDirection, rec.normal);
                else
                    direction = GRay::Math::refract(unitDirection, rec.normal, refractionRatio);

                scattered = GRay::Math::Ray(rec.p, direction);
                return true;
            }
        public:
            double ir; //Index of refraction
        };
    }
}