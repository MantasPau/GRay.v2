#pragma once
#include <cmath>
#include <iostream>
#include <GRay/rtweekend.hpp>

namespace GRay
{
    namespace Math
    {
        using namespace GRay::Utils;
        class Vec3
        {
        public:
            Vec3() : e{ 0, 0, 0 } {}
            Vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

            double x() const { return e[0]; }
            double y() const { return e[1]; }
            double z() const { return e[2]; }

            Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
            double operator[](int i) const { return e[i]; }
            double& operator[](int i) { return e[i]; }

            Vec3& operator+=(const Vec3& v)
            {
                e[0] += v.e[0];
                e[1] += v.e[1];
                e[2] += v.e[2];
                return *this;
            }

            Vec3& operator*=(const double t)
            {
                e[0] *= t;
                e[1] *= t;
                e[2] *= t;
                return *this;
            }

            Vec3& operator/=(const double t)
            {
                return *this *= 1 / t;
            }

            double length() const
            {
                return sqrt(lenghtSquared());
            }

            double lenghtSquared() const
            {
                return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
            }

            bool nearZero() const
            {
                const double s = 1e-8;
                return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
            }
        public:
            double e[3];
        };

        using Point3 = Vec3;
        using Color = Vec3;

        inline std::ostream& operator<<(std::ostream& out, const Vec3& v)
        {
            return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
        }

        inline Vec3 operator+(const Vec3& u, const Vec3& v)
        {
            return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
        }

        inline Vec3 operator-(const Vec3& u, const Vec3& v)
        {
            return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
        }

        inline Vec3 operator*(const Vec3& u, const Vec3& v)
        {
            return Vec3(v.e[0] * u.e[0], v.e[1] * u.e[1], v.e[2] * u.e[2]);
        }

        inline Vec3 operator*(double t, const Vec3& v)
        {
            return Vec3(v.e[0] * t, v.e[1] * t, v.e[2] * t);
        }

        inline Vec3 operator*(const Vec3& v, double t)
        {
            return t * v;
        }

        inline Vec3 operator/(const Vec3& v, double t)
        {
            return (1 / t) * v;
        }

        inline double dot(const Vec3& u, const Vec3& v)
        {
            return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
        }

        inline Vec3 cross(const Vec3& u, const Vec3& v)
        {
            return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
        }

        inline Vec3 unitVector(const Vec3& v)
        {
            return v / v.length();
        }

        inline static Vec3 random()
        {
            return Vec3(randomDouble(), randomDouble(), randomDouble());
        }

        inline static Vec3 random(double min, double max)
        {
            return Vec3(randomDouble(min, max), randomDouble(min, max), randomDouble(min, max));
        }

        inline Vec3 randomInUnitSphere()
        {
            while (true)
            {
                Vec3 p = random(-1, 1);
                if (p.lenghtSquared() >= 1) continue;
                return p;
            }
        }

        inline Vec3 randomUnitVector()
        {
            return unitVector(randomInUnitSphere());
        }

        inline Vec3 randomInHemisphere(const Vec3& normal)
        {
            Vec3 inUnitSphere = randomInUnitSphere();
            if (dot(inUnitSphere, normal) > 0.0)
                return inUnitSphere;
            else
                return -inUnitSphere;
        }

        inline Vec3 randomInUnitDisc()
        {
            while (true)
            {
                Vec3 p = Vec3(randomDouble(-1, 1), randomDouble(-1, 1), 0);
                if (p.lenghtSquared() >= 1) continue;
                return p;
            }
        }

        inline Vec3 reflect(const Vec3& v, const Vec3& n)
        {
            return v - 2 * dot(v, n) * n;
        }

        Vec3 refract(const Vec3& uv, const Vec3& n, double etaiOverEtat)
        {
            double cosTheta = fmin(dot(-uv, n), 1.0);
            Vec3 rOutPerp = etaiOverEtat * (uv + cosTheta * n);
            Vec3 rOutParallel = -sqrt(fabs(1.0 - rOutPerp.lenghtSquared())) * n;
            return rOutPerp + rOutParallel;
        }
    }
}