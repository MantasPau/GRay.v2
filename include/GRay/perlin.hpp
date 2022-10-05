#pragma once

#include <GRay/rtweekend.hpp>

namespace GRay::Materials
{
    class Perlin
    {
    public:
        Perlin()
        {
            ranVec = new Math::Vec3[pointCount];
            for (int i = 0; i < pointCount; i++)
                ranVec[i] = Math::randomUnitVector();
            permX = perlinGeneratePerm();
            permY = perlinGeneratePerm();
            permZ = perlinGeneratePerm();
        }

        ~Perlin()
        {
            delete[] ranVec;
            delete[] permX;
            delete[] permY;
            delete[] permZ;
        }

        double turb(const Math::Point3& p, int depth = 7) const
        {
            double accum = 0.0;
            auto tempP = p;
            double weight = 1.0;

            for (int i = 0; i < depth; ++i)
            {
                accum += weight * noise(tempP);
                weight *= 0.5;
                tempP *= 2;
            }

            return fabs(accum);
        }

        double noise(const Math::Point3& p) const
        {
            double u = p.x() - floor(p.x());
            double v = p.y() - floor(p.y());
            double w = p.z() - floor(p.z());
            
            int i = static_cast<int>(floor(p.x()));
            int j = static_cast<int>(floor(p.y()));
            int k = static_cast<int>(floor(p.z()));
            Math::Vec3 c[2][2][2];

            for (int di = 0; di < 2; ++di)
                for (int dj = 0; dj < 2; ++dj)
                    for (int dk = 0; dk < 2; ++dk)
                        c[di][dj][dk] = ranVec[permX[(i + di) & 255] ^ 
                                               permY[(j + dj) & 255] ^ 
                                               permZ[(k + dk) & 255]];
            return trilinearInterp(c, u, v, w);
        }

    private:
        static const int pointCount = 256;
        Math::Vec3* ranVec;
        int* permX;
        int* permY;
        int* permZ;

        static int* perlinGeneratePerm()
        {
            auto p = new int[pointCount];
            for (int i = 0; i < pointCount; ++i)
                p[i] = i;
            permute(p, pointCount);
            return p;
        }

        static void permute(int* p, int n)
        {
            for (int i = n-1; i > 0; --i)
            {
                int target = Utils::randomInt(0, i);
                int tmp = p[i];
                p[i] = p[target];
                p[target] = tmp;
            }
        }

        static double trilinearInterp(Math::Vec3 c[2][2][2], double u, double v, double w)
        {
            double uu = u*u * (3 - 2*u);
            double vv = v*v * (3 - 2*v);
            double ww = w*w * (3 - 2*w);
            double accum = 0.0;

            for (int i = 0; i < 2; ++i)
                for (int j = 0; j < 2; ++j)
                    for (int k = 0; k < 2; ++k)
                    {
                        Math::Vec3 weightV(u-i, v-j, w-k);
                        accum += (i * uu + (1 - i) * (1 - uu)) *
                                 (j * vv + (1 - j) * (1 - vv)) *
                                 (k * ww + (1 - k) * (1 - ww)) * Math::dot(c[i][j][k], weightV);
                    }
            return accum;
        }
    };
}