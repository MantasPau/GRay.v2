#pragma once

#include <GRay/rtweekend.hpp>
#include <GRay/math.hpp>
#include <GRay/ray.hpp>
#include <GRay/hittable.hpp>
#include <GRay/texture.hpp>

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
        virtual Math::Color emitted(double u, double v, const Math::Point3& p) const {return Math::Color(0, 0, 0);}
    };

    namespace Materials
    {
        class Lambertian : public Material
        {
        public:
            Lambertian(const GRay::Math::Color& a) : albedo{make_shared<SolidColor>(a)} {}
            Lambertian(shared_ptr<Texture> a) : albedo{a} {}
            bool scatter(const GRay::Math::Ray& r_in, const GRay::Math::hitRecord& rec, GRay::Math::Color& attenuation, GRay::Math::Ray& scattered) const override
            {
                GRay::Math::Vec3 scatterDirection = rec.normal + GRay::Math::randomUnitVector();
                if (scatterDirection.nearZero())
                    scatterDirection = rec.normal;
                scattered = GRay::Math::Ray(rec.p, scatterDirection, r_in.time());                
                attenuation = albedo->value(rec.u, rec.v, rec.p);
                return true;
            }
        public:
            shared_ptr<Texture> albedo;
        };

        class Metal : public Material
        {
        public:
            Metal(const GRay::Math::Color& a, double f) : albedo{a}, fuzz{f} {}

            bool scatter(const GRay::Math::Ray& r_in, const GRay::Math::hitRecord& rec, GRay::Math::Color& attenuation, GRay::Math::Ray& scattered) const override
            {
                GRay::Math::Vec3 reflected = GRay::Math::reflect(GRay::Math::unitVector(r_in.direction()), rec.normal);
                scattered = GRay::Math::Ray(rec.p, reflected + fuzz * GRay::Math::randomInUnitSphere(), r_in.time());
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
            bool scatter(const GRay::Math::Ray& r_in, const GRay::Math::hitRecord& rec, GRay::Math::Color& attenuation, GRay::Math::Ray& scattered) const override
            {
                attenuation = GRay::Math::Color(1.0, 1.0, 1.0);
                double refractionRatio = rec.frontFace ? (1.0 / ir) : ir;
                GRay::Math::Vec3 unitDirection = GRay::Math::unitVector(r_in.direction());
                double cosTheta = fmin(GRay::Math::dot(-unitDirection, rec.normal), 1.0);
                double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

                bool cannotRefract = refractionRatio * sinTheta > 1.0;
                GRay::Math::Vec3 direction;
                if(cannotRefract || reflectance(cosTheta, refractionRatio) > Utils::randomDouble())
                    direction = GRay::Math::reflect(unitDirection, rec.normal);
                else
                    direction = GRay::Math::refract(unitDirection, rec.normal, refractionRatio);

                scattered = GRay::Math::Ray(rec.p, direction, r_in.time());
                return true;
            }
        public:
            double ir; //Index of refraction

        private:
            static double reflectance(double consine, double refIdx)
            {
                //shlick's approximation for reflectance (fresnell)
                double r0 = (1 - refIdx) / (1 + refIdx);
                r0 = r0*r0;
                double x = 1 - consine;
                return r0 + (1 - r0) * x*x*x*x*x;
            }
        };

        class DiffuseLight : public Material
        {
        public:
            DiffuseLight(shared_ptr<Texture> a, double att = 1.0) : emit{a}, attenuation{att} {}
            DiffuseLight(Math::Color c, double att = 1.0) : emit{make_shared<SolidColor>(c)}, attenuation{att} {}

            bool scatter(const GRay::Math::Ray& r_in, const GRay::Math::hitRecord& rec, GRay::Math::Color& attenuation, GRay::Math::Ray& scattered) const override
            {
                return false;                
            }

            Math::Color emitted(double u, double v, const Math::Point3& p) const override
            {
                return attenuation * emit->value(u, v, p);
            }
        public:
            shared_ptr<Texture> emit;
            double attenuation;
        };

        class Isotropic : public Material
        {
        public:
            Isotropic(Math::Color c) : albedo{make_shared<SolidColor>(c)} {}
            Isotropic(shared_ptr<Texture> t) : albedo{t} {}
            bool scatter(const GRay::Math::Ray& r_in, const GRay::Math::hitRecord& rec, GRay::Math::Color& attenuation, GRay::Math::Ray& scattered) const override
            {
                scattered = Math::Ray(rec.p, Math::randomInUnitSphere(), r_in.time());
                attenuation = albedo->value(rec.u, rec.v, rec.p);
                return true;
            }
        public:
            shared_ptr<Texture> albedo;
        };
    }
}